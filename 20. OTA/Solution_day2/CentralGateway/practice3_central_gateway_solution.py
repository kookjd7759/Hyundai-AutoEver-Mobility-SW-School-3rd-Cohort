import base64
import getpass
import json
import os

import paho.mqtt.client as mqtt


NOTICE_TOPIC = 'ota/update/notice'
RESPONSE_TOPIC = 'ota/update/response'
FILE_TOPIC = 'ota/update/file'


def parse_version(version):
    parts = version.split('.')
    if not parts or any(not part.isdigit() for part in parts):
        raise ValueError(f'Invalid version: {version}')
    return tuple(int(part) for part in parts)


def is_newer(candidate, current):
    candidate_parts = parse_version(candidate)
    current_parts = parse_version(current)
    width = max(len(candidate_parts), len(current_parts))
    candidate_parts += (0,) * (width - len(candidate_parts))
    current_parts += (0,) * (width - len(current_parts))
    return candidate_parts > current_parts


def on_connect(client, userdata, flags, reason_code):
    if reason_code == 0:
        client.subscribe(NOTICE_TOPIC, qos=1)
        client.subscribe(FILE_TOPIC, qos=1)
        print('Central Gateway is waiting for versioned updates.')


def on_message(client, userdata, message):
    try:
        payload = json.loads(message.payload.decode('utf-8'))
    except (UnicodeDecodeError, json.JSONDecodeError) as exc:
        print('Invalid message:', exc)
        return

    if message.topic == NOTICE_TOPIC:
        try:
            update_version = payload['version']
            if not is_newer(update_version, userdata['current_version']):
                reason = (
                    f'{update_version} is not newer than '
                    f"{userdata['current_version']}"
                )
                response = {
                    'type': 'update_response',
                    'update_id': payload.get('update_id'),
                    'status': 'ignored',
                    'reason': reason,
                }
                client.publish(RESPONSE_TOPIC, json.dumps(response), qos=1)
                print('Update ignored automatically:', reason)
                return
        except (KeyError, ValueError) as exc:
            print('Invalid update notice:', exc)
            return

        userdata['accepted_update_id'] = payload.get('update_id')
        userdata['accepted_version'] = update_version
        response = {
            'type': 'update_response',
            'update_id': payload.get('update_id'),
            'status': 'accepted',
        }
        client.publish(RESPONSE_TOPIC, json.dumps(response), qos=1)
        print('Higher version accepted:', update_version)
        return

    if payload.get('update_id') != userdata.get('accepted_update_id'):
        return

    try:
        file_bytes = base64.b64decode(payload['data'], validate=True)
        filename = os.path.basename(payload.get('filename', 'update.bin'))
        os.makedirs(userdata['output_dir'], exist_ok=True)
        output_path = os.path.join(userdata['output_dir'], filename)
        with open(output_path, 'wb') as output_file:
            output_file.write(file_bytes)
        userdata['current_version'] = userdata['accepted_version']
        print(
            f"Update saved: {output_path}, "
            f"current version={userdata['current_version']}"
        )
    except (KeyError, ValueError) as exc:
        print('File save failed:', exc)


def main():
    print('=== Practice 3 Central Gateway ===')
    broker = input('Broker IP [localhost]: ').strip() or 'localhost'
    port = int(input('Broker Port [1883]: ').strip() or '1883')
    current_version = input('Current Version [1.0.0]: ').strip() or '1.0.0'
    output_dir = input('Download Directory [received_updates]: ').strip() or 'received_updates'
    username = input('MQTT Username (anonymous이면 Enter): ').strip()
    password = getpass.getpass('MQTT Password: ') if username else None

    try:
        parse_version(current_version)
    except ValueError as exc:
        print(exc)
        return

    userdata = {
        'current_version': current_version,
        'output_dir': output_dir,
        'accepted_update_id': None,
        'accepted_version': None,
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
