import base64
import getpass
import json
import os
import time

import paho.mqtt.client as mqtt


NOTICE_TOPIC = 'ota/update/notice'
FILE_TOPIC = 'ota/update/file'


def on_connect(client, userdata, flags, reason_code):
    if reason_code == 0:
        print('Update Server connected to MQTT Broker.')
    else:
        print('MQTT connection failed:', reason_code)


def publish_and_wait(client, topic, payload):
    info = client.publish(topic, json.dumps(payload), qos=1)
    info.wait_for_publish()


def main():
    print('=== Practice 1 Update Server ===')
    broker = input('Broker IP [localhost]: ').strip() or 'localhost'
    port = int(input('Broker Port [1883]: ').strip() or '1883')
    target_ecu = input('Target ECU [powertrain]: ').strip() or 'powertrain'
    file_path = input('Update File Path: ').strip()
    username = input('MQTT Username (anonymous이면 Enter): ').strip()
    password = getpass.getpass('MQTT Password: ') if username else None

    if not os.path.isfile(file_path):
        print('File not found:', file_path)
        return

    with open(file_path, 'rb') as update_file:
        file_bytes = update_file.read()

    client = mqtt.Client()
    client.on_connect = on_connect
    if username:
        client.username_pw_set(username, password)

    try:
        client.connect(broker, port)
        client.loop_start()

        notice = {
            'type': 'update_notice',
            'target_ecu': target_ecu,
            'filename': os.path.basename(file_path),
            'size': len(file_bytes),
        }
        publish_and_wait(client, NOTICE_TOPIC, notice)
        print('Update notice sent.')

        time.sleep(1)
        file_message = {
            'type': 'update_file',
            'target_ecu': target_ecu,
            'filename': os.path.basename(file_path),
            'data': base64.b64encode(file_bytes).decode('ascii'),
        }
        publish_and_wait(client, FILE_TOPIC, file_message)
        print('Update file sent.')
    except Exception as exc:
        print('Publish error:', exc)
    finally:
        client.loop_stop()
        client.disconnect()


if __name__ == '__main__':
    main()
