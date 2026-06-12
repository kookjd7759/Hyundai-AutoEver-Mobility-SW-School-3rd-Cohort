import base64
import getpass
import io
import json
import os
import zipfile

import paho.mqtt.client as mqtt


PACKAGE_TOPIC = 'ota/challenge/package'


def extract_package(package_bytes, output_dir):
    os.makedirs(output_dir, exist_ok=True)
    output_root = os.path.abspath(output_dir)

    with zipfile.ZipFile(io.BytesIO(package_bytes)) as package:
        for member in package.infolist():
            output_path = os.path.abspath(
                os.path.join(output_root, member.filename)
            )
            if os.path.commonpath([output_root, output_path]) != output_root:
                raise ValueError(f'Unsafe package path: {member.filename}')
        package.extractall(output_root)
        return [member.filename for member in package.infolist() if not member.is_dir()]


def on_connect(client, userdata, flags, reason_code):
    if reason_code == 0:
        client.subscribe(PACKAGE_TOPIC, qos=1)
        print('Waiting for an OTA package...')


def on_message(client, userdata, message):
    try:
        payload = json.loads(message.payload.decode('utf-8'))
        package_bytes = base64.b64decode(payload['data'], validate=True)
        filenames = extract_package(package_bytes, userdata['output_dir'])
        print('OTA package separated successfully:')
        for filename in filenames:
            print(' -', filename)
    except Exception as exc:
        print('Package extraction failed:', exc)


def main():
    print('=== Challenge 1 Central Gateway: Package Split ===')
    broker = input('Broker IP [localhost]: ').strip() or 'localhost'
    port = int(input('Broker Port [1883]: ').strip() or '1883')
    output_dir = input('Package Output Directory [ota_package]: ').strip() or 'ota_package'
    username = input('MQTT Username (anonymous이면 Enter): ').strip()
    password = getpass.getpass('MQTT Password: ') if username else None

    client = mqtt.Client(userdata={'output_dir': output_dir})
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
