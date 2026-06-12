import base64
import getpass
import io
import json
import os
import zipfile

import paho.mqtt.client as mqtt


PACKAGE_TOPIC = 'ota/challenge/package'


def main():
    print('=== Challenge 1 Update Server: OTA Package ===')
    broker = input('Broker IP [localhost]: ').strip() or 'localhost'
    port = int(input('Broker Port [1883]: ').strip() or '1883')
    file_paths = [
        path.strip()
        for path in input('Update File Paths (comma separated): ').split(',')
        if path.strip()
    ]
    username = input('MQTT Username (anonymous이면 Enter): ').strip()
    password = getpass.getpass('MQTT Password: ') if username else None

    if not file_paths or any(not os.path.isfile(path) for path in file_paths):
        print('One or more update files do not exist.')
        return

    package_buffer = io.BytesIO()
    with zipfile.ZipFile(package_buffer, 'w', zipfile.ZIP_DEFLATED) as package:
        for file_path in file_paths:
            package.write(file_path, arcname=os.path.basename(file_path))

    package_bytes = package_buffer.getvalue()
    payload = {
        'type': 'ota_package',
        'filename': 'ota_package.zip',
        'file_count': len(file_paths),
        'data': base64.b64encode(package_bytes).decode('ascii'),
    }

    client = mqtt.Client()
    if username:
        client.username_pw_set(username, password)
    client.connect(broker, port)
    client.loop_start()
    try:
        info = client.publish(PACKAGE_TOPIC, json.dumps(payload), qos=1)
        info.wait_for_publish()
        print(
            f'OTA package sent: {len(file_paths)} files, '
            f'{len(package_bytes)} bytes'
        )
    finally:
        client.loop_stop()
        client.disconnect()


if __name__ == '__main__':
    main()
