import base64
import getpass
import hashlib
import json
import os
import threading
import uuid

import paho.mqtt.client as mqtt


NOTICE_TOPIC = 'ota/update/notice'
RESPONSE_TOPIC = 'ota/update/response'
FILE_TOPIC = 'ota/update/file'
RESULT_TOPIC = 'ota/update/result'


def send_file(client, userdata):
    payload = {
        'type': 'update_file',
        'update_id': userdata['update_id'],
        'target_ecu': userdata['target_ecu'],
        'version': userdata['version'],
        'filename': os.path.basename(userdata['file_path']),
        'size': len(userdata['file_bytes']),
        'sha256': userdata['sha256'],
        'data': base64.b64encode(userdata['file_bytes']).decode('ascii'),
    }
    client.publish(FILE_TOPIC, json.dumps(payload), qos=1)
    print(f"Update file sent. retry={userdata['retry_count']}")


def on_connect(client, userdata, flags, reason_code):
    if reason_code != 0:
        print('MQTT connection failed:', reason_code)
        return
    client.subscribe(RESPONSE_TOPIC, qos=1)
    client.subscribe(RESULT_TOPIC, qos=1)
    notice = {
        'type': 'update_notice',
        'update_id': userdata['update_id'],
        'target_ecu': userdata['target_ecu'],
        'version': userdata['version'],
        'filename': os.path.basename(userdata['file_path']),
        'size': len(userdata['file_bytes']),
        'sha256': userdata['sha256'],
    }
    client.publish(NOTICE_TOPIC, json.dumps(notice), qos=1)
    print('Update notice sent.')


def on_message(client, userdata, message):
    try:
        payload = json.loads(message.payload.decode('utf-8'))
    except (UnicodeDecodeError, json.JSONDecodeError):
        return
    if payload.get('update_id') != userdata['update_id']:
        return

    if message.topic == RESPONSE_TOPIC:
        if payload.get('status') == 'accepted':
            send_file(client, userdata)
        elif payload.get('status') == 'ignored':
            print('Update ignored:', payload.get('reason'))
            userdata['finished'].set()
        return

    if payload.get('status') == 'success':
        print('Central Gateway confirmed the download.')
        userdata['finished'].set()
    elif payload.get('status') == 'failed':
        print('Download failed:', payload.get('reason'))
        if userdata['retry_count'] < userdata['max_retries']:
            userdata['retry_count'] += 1
            send_file(client, userdata)
        else:
            print('Maximum retry count reached.')
            userdata['finished'].set()


def main():
    print('=== Practice 4 Update Server ===')
    broker = input('Broker IP [localhost]: ').strip() or 'localhost'
    port = int(input('Broker Port [1883]: ').strip() or '1883')
    target_ecu = input('Target ECU [powertrain]: ').strip() or 'powertrain'
    version = input('Update Version [1.1.0]: ').strip() or '1.1.0'
    file_path = input('Update File Path: ').strip()
    max_retries = int(input('Maximum Retries [2]: ').strip() or '2')
    username = input('MQTT Username (anonymous이면 Enter): ').strip()
    password = getpass.getpass('MQTT Password: ') if username else None

    if not os.path.isfile(file_path):
        print('File not found:', file_path)
        return
    with open(file_path, 'rb') as update_file:
        file_bytes = update_file.read()

    userdata = {
        'update_id': uuid.uuid4().hex[:12],
        'target_ecu': target_ecu,
        'version': version,
        'file_path': file_path,
        'file_bytes': file_bytes,
        'sha256': hashlib.sha256(file_bytes).hexdigest(),
        'retry_count': 0,
        'max_retries': max_retries,
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
