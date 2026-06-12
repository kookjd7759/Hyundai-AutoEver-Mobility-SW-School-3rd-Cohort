import base64
import getpass
import json
import os
import uuid

import paho.mqtt.client as mqtt


NOTICE_TOPIC = 'ota/update/notice'
RESPONSE_TOPIC = 'ota/update/response'
FILE_TOPIC = 'ota/update/file'


def publish_file(client, update_id, requester_id, target_ecu, file_path):
    with open(file_path, 'rb') as update_file:
        file_bytes = update_file.read()
    payload = {
        'type': 'update_file',
        'update_id': update_id,
        'recipient_id': requester_id,
        'target_ecu': target_ecu,
        'filename': os.path.basename(file_path),
        'data': base64.b64encode(file_bytes).decode('ascii'),
    }
    client.publish(FILE_TOPIC, json.dumps(payload), qos=1)
    print(f'Update file sent to requester: {requester_id}')


def on_connect(client, userdata, flags, reason_code):
    if reason_code == 0:
        print('Update Server connected to MQTT Broker.')
    else:
        print('MQTT connection failed:', reason_code)


def on_message(client, userdata, message):
    try:
        payload = json.loads(message.payload.decode('utf-8'))
    except (UnicodeDecodeError, json.JSONDecodeError):
        return
    if payload.get('update_id') != userdata['update_id']:
        return

    requester_id = payload.get('requester_id')
    if not requester_id:
        print('Ignored response without requester_id.')
        return

    if payload.get('status') == 'accepted':
        print(f'Update accepted by requester: {requester_id}')
        publish_file(
            client,
            userdata['update_id'],
            requester_id,
            userdata['target_ecu'],
            userdata['file_path'],
        )
    elif payload.get('status') == 'rejected':
        print(f'Update rejected by requester: {requester_id}')


def main():
    print('=== Practice 2 Update Server ===')
    broker = input('Broker IP [localhost]: ').strip() or 'localhost'
    port = int(input('Broker Port [1883]: ').strip() or '1883')
    target_ecu = input('Target ECU [powertrain]: ').strip() or 'powertrain'
    file_path = input('Update File Path: ').strip()
    username = input('MQTT Username (anonymous이면 Enter): ').strip()
    password = getpass.getpass('MQTT Password: ') if username else None

    if not os.path.isfile(file_path):
        print('File not found:', file_path)
        return

    userdata = {
        'update_id': uuid.uuid4().hex[:12],
        'target_ecu': target_ecu,
        'file_path': file_path,
    }
    client = mqtt.Client(userdata=userdata)
    client.on_connect = on_connect
    client.on_message = on_message
    if username:
        client.username_pw_set(username, password)

    client.connect(broker, port)
    try:
        while not client.is_connected():
            client.loop(timeout=1.0)

        notice = {
            'type': 'update_notice',
            'update_id': userdata['update_id'],
            'target_ecu': userdata['target_ecu'],
            'filename': os.path.basename(userdata['file_path']),
        }

        notice_info = client.publish(
            NOTICE_TOPIC,
            json.dumps(notice),
            qos=1,
        )
        client.subscribe(RESPONSE_TOPIC, qos=1)
        while not notice_info.is_published():
            client.loop(timeout=1.0)

        print('Update notice sent. Waiting for vehicle requests...')
        print('Press Ctrl+C to stop the Update Server.')
        while True:
            client.loop(timeout=1.0)
    except KeyboardInterrupt:
        print('Stopped by user.')
    finally:
        client.disconnect()


if __name__ == '__main__':
    main()
