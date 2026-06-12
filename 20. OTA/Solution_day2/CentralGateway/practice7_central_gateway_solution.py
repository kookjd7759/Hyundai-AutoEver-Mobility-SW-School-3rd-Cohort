import base64
import getpass
import hashlib
import json
import os
import struct
import sys
import time

import paho.mqtt.client as mqtt


PROJECT_ROOT = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
if PROJECT_ROOT not in sys.path:
    sys.path.insert(0, PROJECT_ROOT)

from CANTransceiver.caninterface import CANInterface


NOTICE_TOPIC = 'ota/update/notice'
RESPONSE_TOPIC = 'ota/update/response'
FILE_TOPIC = 'ota/update/file'
RESULT_TOPIC = 'ota/update/result'

# 실제 차량처럼 ECU별 CAN ID는 Central Gateway의 라우팅 테이블에서 관리한다.
ECU_CAN_ROUTES = {
    'powertrain': {'start': 0x700, 'data': 0x701, 'end': 0x702, 'result': 0x703},
    'body': {'start': 0x710, 'data': 0x711, 'end': 0x712, 'result': 0x713},
    'adas': {'start': 0x720, 'data': 0x721, 'end': 0x722, 'result': 0x723},
}


def parse_version(version):
    parts = version.split('.')
    if not parts or any(not part.isdigit() for part in parts):
        raise ValueError(f'Invalid version: {version}')
    return tuple(int(part) for part in parts)


def is_newer(candidate, current):
    candidate_parts = parse_version(candidate)
    current_parts = parse_version(current)
    width = max(len(candidate_parts), len(current_parts))
    return (
        candidate_parts + (0,) * (width - len(candidate_parts))
        > current_parts + (0,) * (width - len(current_parts))
    )


def publish_json(client, topic, payload):
    client.publish(topic, json.dumps(payload), qos=1)


def publish_result(client, update_id, status, reason=''):
    publish_json(client, RESULT_TOPIC, {
        'type': 'update_result',
        'update_id': update_id,
        'status': status,
        'reason': reason,
    })


def send_file_to_ecu(can_bus, route, file_bytes, timeout):
    transfer_id = int.from_bytes(os.urandom(2), 'big') or 1
    chunk_count = (len(file_bytes) + 3) // 4
    if chunk_count > 0xFFFF:
        raise ValueError('File is too large for this training CAN protocol.')

    # Start: transfer ID(2) + expected file size(4)
    can_bus.send(
        route['start'],
        struct.pack('>HI', transfer_id, len(file_bytes)),
    )

    # Data: transfer ID(2) + sequence(2) + data(4)
    for sequence, offset in enumerate(range(0, len(file_bytes), 4)):
        frame = (
            struct.pack('>HH', transfer_id, sequence)
            + file_bytes[offset:offset + 4]
        )
        can_bus.send(route['data'], frame)
        time.sleep(0.01)

    # End: transfer ID(2) + total chunk count(2)
    can_bus.send(
        route['end'],
        struct.pack('>HH', transfer_id, chunk_count),
    )
    print('5. Update file forwarded from Central Gateway to ECU.')

    deadline = time.monotonic() + timeout
    while time.monotonic() < deadline:
        frame = can_bus.receive(timeout=0.5)
        if frame is None or frame['id'] != route['result']:
            continue
        if len(frame['data']) < 3:
            continue
        result_id, status = struct.unpack('>HB', frame['data'][:3])
        if result_id != transfer_id:
            continue
        if status == 1:
            return
        raise RuntimeError('ECU reported file verification failure.')
    raise TimeoutError('ECU result timeout.')


def on_connect(client, userdata, flags, reason_code):
    if reason_code == 0:
        client.subscribe(NOTICE_TOPIC, qos=1)
        client.subscribe(FILE_TOPIC, qos=1)
        print('Central Gateway is waiting for OTA messages.')
    else:
        print('MQTT connection failed:', reason_code)


def on_message(client, userdata, message):
    try:
        payload = json.loads(message.payload.decode('utf-8'))
    except (UnicodeDecodeError, json.JSONDecodeError) as exc:
        print('Invalid MQTT message:', exc)
        return

    if message.topic == NOTICE_TOPIC:
        try:
            target_ecu = payload['target_ecu']
            if target_ecu not in ECU_CAN_ROUTES:
                raise ValueError(f'Unsupported target ECU: {target_ecu}')
            if not is_newer(payload['version'], userdata['current_version']):
                raise ValueError('same or lower version')
        except (KeyError, ValueError) as exc:
            publish_json(client, RESPONSE_TOPIC, {
                'update_id': payload.get('update_id'),
                'status': 'ignored',
                'reason': str(exc),
            })
            print('Update ignored:', exc)
            return

        userdata['notice'] = payload
        publish_json(client, RESPONSE_TOPIC, {
            'update_id': payload['update_id'],
            'status': 'accepted',
        })
        print(
            f"2. Update accepted: target={target_ecu}, "
            f"version={payload['version']}"
        )
        return

    notice = userdata.get('notice')
    if not notice or payload.get('update_id') != notice.get('update_id'):
        return

    update_id = payload.get('update_id')
    try:
        file_bytes = base64.b64decode(payload['data'], validate=True)
        if len(file_bytes) != int(payload['size']):
            raise ValueError('MQTT file size mismatch')
        if hashlib.sha256(file_bytes).hexdigest() != payload['sha256']:
            raise ValueError('MQTT SHA-256 mismatch')

        filename = os.path.basename(payload.get('filename', 'update.bin'))
        os.makedirs(userdata['download_dir'], exist_ok=True)
        download_path = os.path.join(userdata['download_dir'], filename)
        with open(download_path, 'wb') as output_file:
            output_file.write(file_bytes)
        print('4. Central Gateway verified and saved:', download_path)

        route = ECU_CAN_ROUTES[payload['target_ecu']]
        send_file_to_ecu(
            userdata['can_bus'],
            route,
            file_bytes,
            userdata['can_timeout'],
        )
        userdata['current_version'] = payload['version']
        userdata['notice'] = None
        print('6. ECU verification response received.')
        publish_result(client, update_id, 'success')
    except Exception as exc:
        print('OTA forwarding failed:', exc)
        publish_result(client, update_id, 'failed', str(exc))


def main():
    print('=== Practice 7 Central Gateway: End-to-End OTA ===')
    broker = input('Broker IP [localhost]: ').strip() or 'localhost'
    port = int(input('Broker Port [1883]: ').strip() or '1883')
    current_version = input('Current Version [1.0.0]: ').strip() or '1.0.0'
    download_dir = input('Download Directory [received_updates]: ').strip() or 'received_updates'
    can_channel = input('CAN Channel [can0]: ').strip() or 'can0'
    can_bitrate = int(input('CAN Bitrate [1000000]: ').strip() or '1000000')
    can_timeout = float(input('ECU Response Timeout [15]: ').strip() or '15')
    username = input('MQTT Username (anonymous이면 Enter): ').strip()
    password = getpass.getpass('MQTT Password: ') if username else None

    try:
        parse_version(current_version)
    except ValueError as exc:
        print(exc)
        return

    can_bus = CANInterface(channel=can_channel, bitrate=can_bitrate)
    can_bus.setup()
    userdata = {
        'current_version': current_version,
        'download_dir': download_dir,
        'can_bus': can_bus,
        'can_timeout': can_timeout,
        'notice': None,
    }
    client = mqtt.Client(userdata=userdata)
    client.on_connect = on_connect
    client.on_message = on_message
    if username:
        client.username_pw_set(username, password)

    try:
        client.connect(broker, port)
        client.loop_forever()
    except KeyboardInterrupt:
        print('Stopped by user.')
    finally:
        client.disconnect()
        can_bus.close()


if __name__ == '__main__':
    main()
