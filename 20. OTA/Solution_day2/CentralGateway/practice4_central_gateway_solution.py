import base64
import getpass
import hashlib
import json
import os

import paho.mqtt.client as mqtt


NOTICE_TOPIC = 'ota/update/notice'
RESPONSE_TOPIC = 'ota/update/response'
FILE_TOPIC = 'ota/update/file'
RESULT_TOPIC = 'ota/update/result'


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


def publish_result(client, update_id, status, reason=''):
    payload = {
        'type': 'update_result',
        'update_id': update_id,
        'status': status,
        'reason': reason,
    }
    client.publish(RESULT_TOPIC, json.dumps(payload), qos=1)


def on_connect(client, userdata, flags, reason_code):
    if reason_code == 0:
        client.subscribe(NOTICE_TOPIC, qos=1)
        client.subscribe(FILE_TOPIC, qos=1)
        print('Central Gateway is waiting for verified updates.')


def on_message(client, userdata, message):
    try:
        payload = json.loads(message.payload.decode('utf-8'))
    except (UnicodeDecodeError, json.JSONDecodeError) as exc:
        print('Invalid message:', exc)
        return

    if message.topic == NOTICE_TOPIC:
        try:
            if not is_newer(payload['version'], userdata['current_version']):
                reason = 'same or lower version'
                client.publish(RESPONSE_TOPIC, json.dumps({
                    'update_id': payload.get('update_id'),
                    'status': 'ignored',
                    'reason': reason,
                }), qos=1)
                print('Update ignored:', reason)
                return
        except (KeyError, ValueError) as exc:
            print('Invalid notice:', exc)
            return

        userdata['notice'] = payload
        client.publish(RESPONSE_TOPIC, json.dumps({
            'update_id': payload.get('update_id'),
            'status': 'accepted',
        }), qos=1)
        print('Update accepted:', payload['version'])
        return

    notice = userdata.get('notice')
    if not notice or payload.get('update_id') != notice.get('update_id'):
        return

    try:
        file_bytes = base64.b64decode(payload['data'], validate=True)
        if userdata['force_fail_once']:
            userdata['force_fail_once'] = False
            raise ValueError('forced failure for retransmission practice')
        if len(file_bytes) != int(payload['size']):
            raise ValueError('file size mismatch')
        if hashlib.sha256(file_bytes).hexdigest() != payload['sha256']:
            raise ValueError('SHA-256 mismatch')

        filename = os.path.basename(payload.get('filename', 'update.bin'))
        os.makedirs(userdata['output_dir'], exist_ok=True)
        output_path = os.path.join(userdata['output_dir'], filename)
        with open(output_path, 'wb') as output_file:
            output_file.write(file_bytes)
        userdata['current_version'] = payload['version']
        userdata['notice'] = None
        print('Verified update saved:', output_path)
        publish_result(client, payload['update_id'], 'success')
    except (KeyError, ValueError) as exc:
        print('Download verification failed:', exc)
        publish_result(client, payload.get('update_id'), 'failed', str(exc))


def main():
    print('=== Practice 4 Central Gateway ===')
    broker = input('Broker IP [localhost]: ').strip() or 'localhost'
    port = int(input('Broker Port [1883]: ').strip() or '1883')
    current_version = input('Current Version [1.0.0]: ').strip() or '1.0.0'
    output_dir = input('Download Directory [received_updates]: ').strip() or 'received_updates'
    force_fail = input('Force first download failure? (y/n) [n]: ').strip().lower() == 'y'
    username = input('MQTT Username (anonymous이면 Enter): ').strip()
    password = getpass.getpass('MQTT Password: ') if username else None

    userdata = {
        'current_version': current_version,
        'output_dir': output_dir,
        'force_fail_once': force_fail,
        'notice': None,
    }
    client = mqtt.Client(userdata=userdata)
    client.on_connect = on_connect
    client.on_message = on_message
    if username:
        client.username_pw_set(username, password)
    client.connect(broker, port)
    try:
        client.loop_forever()
    except KeyboardInterrupt:
        print('Stopped by user.')


if __name__ == '__main__':
    main()
