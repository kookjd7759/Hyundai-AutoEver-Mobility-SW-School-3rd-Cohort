import base64
import getpass
import json
import os

import paho.mqtt.client as mqtt


NOTICE_TOPIC = 'ota/update/notice'
FILE_TOPIC = 'ota/update/file'


def on_connect(client, userdata, flags, reason_code):
    if reason_code == 0:
        client.subscribe(NOTICE_TOPIC, qos=1)
        client.subscribe(FILE_TOPIC, qos=1)
        print('Central Gateway is waiting for update notice and file.')
    else:
        print('MQTT connection failed:', reason_code)


def on_message(client, userdata, message):
    try:
        payload = json.loads(message.payload.decode('utf-8'))
    except (UnicodeDecodeError, json.JSONDecodeError) as exc:
        print('Invalid message:', exc)
        return

    if message.topic == NOTICE_TOPIC:
        print(
            f"Update notice: target={payload.get('target_ecu')}, "
            f"file={payload.get('filename')}, size={payload.get('size')}"
        )
        return

    try:
        file_bytes = base64.b64decode(payload['data'], validate=True)
        filename = os.path.basename(payload.get('filename', 'update.bin'))
        os.makedirs(userdata['output_dir'], exist_ok=True)
        output_path = os.path.join(userdata['output_dir'], filename)
        with open(output_path, 'wb') as output_file:
            output_file.write(file_bytes)
        print('Update file saved:', output_path)
    except (KeyError, ValueError) as exc:
        print('File save failed:', exc)


def main():
    print('=== Practice 1 Central Gateway ===')
    broker = input('Broker IP [localhost]: ').strip() or 'localhost'
    port = int(input('Broker Port [1883]: ').strip() or '1883')
    output_dir = input('Download Directory [received_updates]: ').strip() or 'received_updates'
    username = input('MQTT Username (anonymous이면 Enter): ').strip()
    password = getpass.getpass('MQTT Password: ') if username else None

    client = mqtt.Client(userdata={'output_dir': output_dir})
    client.on_connect = on_connect
    client.on_message = on_message
    if username:
        client.username_pw_set(username, password)

    try:
        client.connect(broker, port)
        client.loop_forever()
    except KeyboardInterrupt:
        print('Stopped by user.')
    except Exception as exc:
        print('Connection error:', exc)


if __name__ == '__main__':
    main()
