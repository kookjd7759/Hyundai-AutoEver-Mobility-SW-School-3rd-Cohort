import base64
import getpass
import json
import os
import threading
import uuid

import paho.mqtt.client as mqtt


NOTICE_TOPIC = 'ota/update/notice'
RESPONSE_TOPIC = 'ota/update/response'
FILE_TOPIC = 'ota/update/file'


def on_connect(client, userdata, flags, reason_code):
    if reason_code != 0:
        print('MQTT connection failed:', reason_code)
        return
    client.subscribe(RESPONSE_TOPIC, qos=1)
    notice = {
        'type': 'update_notice',
        'update_id': userdata['update_id'],
        'target_ecu': userdata['target_ecu'],
        'version': userdata['version'],
        'filename': os.path.basename(userdata['file_path']),
    }
    client.publish(NOTICE_TOPIC, json.dumps(notice), qos=1)
    print('Versioned update notice sent.')


def on_message(client, userdata, message):
    try:
        response = json.loads(message.payload.decode('utf-8'))
    except (UnicodeDecodeError, json.JSONDecodeError):
        return
    if response.get('update_id') != userdata['update_id']:
        return

    if response.get('status') == 'ignored':
        print('Update ignored:', response.get('reason'))
        userdata['finished'].set()
        return
    if response.get('status') != 'accepted':
        return

    with open(userdata['file_path'], 'rb') as update_file:
        file_bytes = update_file.read()
    payload = {
        'type': 'update_file',
        'update_id': userdata['update_id'],
        'target_ecu': userdata['target_ecu'],
        'version': userdata['version'],
        'filename': os.path.basename(userdata['file_path']),
        'data': base64.b64encode(file_bytes).decode('ascii'),
    }
    info = client.publish(FILE_TOPIC, json.dumps(payload), qos=1)
    info.wait_for_publish()
    print('Higher-version update file sent.')
    userdata['finished'].set()


def main():
    print('=== Practice 3 Update Server ===')
    broker = input('Broker IP [localhost]: ').strip() or 'localhost'
    port = int(input('Broker Port [1883]: ').strip() or '1883')
    target_ecu = input('Target ECU [powertrain]: ').strip() or 'powertrain'
    version = input('Update Version [1.1.0]: ').strip() or '1.1.0'
    file_path = input('Update File Path: ').strip()
    username = input('MQTT Username (anonymous이면 Enter): ').strip()
    password = getpass.getpass('MQTT Password: ') if username else None

    if not os.path.isfile(file_path):
        print('File not found:', file_path)
        return

    userdata = {
        'update_id': uuid.uuid4().hex[:12],
        'target_ecu': target_ecu,
        'version': version,
        'file_path': file_path,
        'finished': threading.Event(),
    }
    client = mqtt.Client(userdata=userdata)
    client.on_connect = on_connect
    client.on_message = on_message
    if username:
        client.username_pw_set(username, password)
    client.connect(broker, port)
    client.loop_start()
    try:
        userdata['finished'].wait()
    except KeyboardInterrupt:
        print('Stopped by user.')
    finally:
        client.loop_stop()
        client.disconnect()


if __name__ == '__main__':
    main()
