import base64
import getpass
import io
import json
import os
import zipfile

import paho.mqtt.client as mqtt


PACKAGE_TOPIC = 'ota/challenge/multi_ecu_package'


def main():
    print('=== Challenge 4 Update Server: Multi-ECU Package ===')
    broker = input('Broker IP [localhost]: ').strip() or 'localhost'
    port = int(input('Broker Port [1883]: ').strip() or '1883')
    print('Enter package items as ecu_name:file_path. Example:')
    print('powertrain:engine.bin,body:body.bin,adas:model.bin')
    item_text = input('Package Items: ').strip()
    username = input('MQTT Username (anonymous이면 Enter): ').strip()
    password = getpass.getpass('MQTT Password: ') if username else None

    items = []
    for raw_item in item_text.split(','):
        if ':' not in raw_item:
            continue
        target_ecu, file_path = raw_item.split(':', 1)
        target_ecu = target_ecu.strip()
        file_path = file_path.strip()
        if target_ecu and os.path.isfile(file_path):
            items.append((target_ecu, file_path))
    if not items:
        print('No valid package items.')
        return

    manifest = {'files': []}
    package_buffer = io.BytesIO()
    with zipfile.ZipFile(package_buffer, 'w', zipfile.ZIP_DEFLATED) as package:
        for index, (target_ecu, file_path) in enumerate(items):
            stored_name = f'payload/{index}_{os.path.basename(file_path)}'
            package.write(file_path, arcname=stored_name)
            manifest['files'].append({
                'target_ecu': target_ecu,
                'path': stored_name,
                'filename': os.path.basename(file_path),
            })
        package.writestr('manifest.json', json.dumps(manifest))

    payload = {
        'type': 'multi_ecu_package',
        'data': base64.b64encode(package_buffer.getvalue()).decode('ascii'),
    }
    client = mqtt.Client()
    if username:
        client.username_pw_set(username, password)
    client.connect(broker, port)
    client.loop_start()
    try:
        info = client.publish(PACKAGE_TOPIC, json.dumps(payload), qos=1)
        info.wait_for_publish()
        print('Multi-ECU package sent:')
        for item in manifest['files']:
            print(f" - {item['filename']} -> {item['target_ecu']}")
    finally:
        client.loop_stop()
        client.disconnect()


if __name__ == '__main__':
    main()
