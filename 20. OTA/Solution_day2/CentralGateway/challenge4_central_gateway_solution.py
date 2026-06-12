import base64
import getpass
import io
import json
import os
import struct
import sys
import time
import zipfile

import paho.mqtt.client as mqtt


PROJECT_ROOT = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
if PROJECT_ROOT not in sys.path:
    sys.path.insert(0, PROJECT_ROOT)

from CANTransceiver.caninterface import CANInterface


PACKAGE_TOPIC = 'ota/challenge/multi_ecu_package'
ECU_CAN_ROUTES = {
    'powertrain': {'start': 0x740, 'name': 0x741, 'data': 0x742, 'end': 0x743, 'result': 0x744},
    'body': {'start': 0x750, 'name': 0x751, 'data': 0x752, 'end': 0x753, 'result': 0x754},
    'adas': {'start': 0x760, 'name': 0x761, 'data': 0x762, 'end': 0x763, 'result': 0x764},
}


def send_to_ecu(can_bus, route, filename, file_bytes, timeout):
    transfer_id = int.from_bytes(os.urandom(2), 'big') or 1
    filename_bytes = os.path.basename(filename).encode('utf-8')
    chunks = [
        file_bytes[offset:offset + 4]
        for offset in range(0, len(file_bytes), 4)
    ]
    if len(filename_bytes) > 255 or len(chunks) > 0xFFFF:
        raise ValueError('Filename or file is too large for the CAN protocol.')

    can_bus.send(
        route['start'],
        struct.pack('>HIB', transfer_id, len(file_bytes), len(filename_bytes)),
    )
    for sequence, offset in enumerate(range(0, len(filename_bytes), 5)):
        can_bus.send(
            route['name'],
            struct.pack('>HB', transfer_id, sequence)
            + filename_bytes[offset:offset + 5],
        )
    for sequence, chunk in enumerate(chunks):
        can_bus.send(
            route['data'],
            struct.pack('>HH', transfer_id, sequence) + chunk,
        )
    can_bus.send(
        route['end'],
        struct.pack('>HH', transfer_id, len(chunks)),
    )

    deadline = time.monotonic() + timeout
    while time.monotonic() < deadline:
        frame = can_bus.receive(timeout=0.5)
        if frame is None or frame['id'] != route['result']:
            continue
        result_id, status = struct.unpack('>HB', frame['data'][:3])
        if result_id == transfer_id:
            if status == 0:
                return
            raise RuntimeError('ECU rejected the file.')
    raise TimeoutError('ECU response timeout.')


def process_package(package_bytes, can_bus, timeout):
    with zipfile.ZipFile(io.BytesIO(package_bytes)) as package:
        manifest = json.loads(package.read('manifest.json').decode('utf-8'))
        for item in manifest['files']:
            target_ecu = item['target_ecu']
            if target_ecu not in ECU_CAN_ROUTES:
                print('Skipping unsupported ECU:', target_ecu)
                continue
            file_bytes = package.read(item['path'])
            print(f"Routing {item['filename']} to {target_ecu} ECU...")
            send_to_ecu(
                can_bus,
                ECU_CAN_ROUTES[target_ecu],
                item['filename'],
                file_bytes,
                timeout,
            )
            print(f"{target_ecu} ECU stored {item['filename']}.")


def on_connect(client, userdata, flags, reason_code):
    if reason_code == 0:
        client.subscribe(PACKAGE_TOPIC, qos=1)
        print('Waiting for a multi-ECU OTA package...')


def on_message(client, userdata, message):
    try:
        payload = json.loads(message.payload.decode('utf-8'))
        package_bytes = base64.b64decode(payload['data'], validate=True)
        process_package(
            package_bytes,
            userdata['can_bus'],
            userdata['timeout'],
        )
        print('All supported package files were distributed.')
    except Exception as exc:
        print('Package distribution failed:', exc)


def main():
    print('=== Challenge 4 Central Gateway: ECU Package Router ===')
    broker = input('Broker IP [localhost]: ').strip() or 'localhost'
    port = int(input('Broker Port [1883]: ').strip() or '1883')
    channel = input('CAN Channel [can0]: ').strip() or 'can0'
    bitrate = int(input('CAN Bitrate [1000000]: ').strip() or '1000000')
    timeout = float(input('ECU Response Timeout [15]: ').strip() or '15')
    username = input('MQTT Username (anonymous이면 Enter): ').strip()
    password = getpass.getpass('MQTT Password: ') if username else None

    can_bus = CANInterface(channel=channel, bitrate=bitrate)
    can_bus.setup()
    client = mqtt.Client(userdata={'can_bus': can_bus, 'timeout': timeout})
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
