import base64
import getpass
import json
import os

import paho.mqtt.client as mqtt


NOTICE_TOPIC = 'ota/update/notice'
RESPONSE_TOPIC = 'ota/update/response'
FILE_TOPIC = 'ota/update/file'


def on_connect(client, userdata, flags, reason_code):
    if reason_code == 0:
        client.subscribe(NOTICE_TOPIC, qos=1)
        client.subscribe(FILE_TOPIC, qos=1)
        print('Central Gateway is waiting for an update.')


def on_message(client, userdata, message):
    try:
        payload = json.loads(message.payload.decode('utf-8'))
    except (UnicodeDecodeError, json.JSONDecodeError) as exc:
        print('Invalid message:', exc)
        return

    if message.topic == NOTICE_TOPIC:
        print(
            f"Update notice received: target={payload.get('target_ecu')}, "
            f"file={payload.get('filename')}"
        )
        answer = input('Accept this update? (y/n) [y]: ').strip().lower() or 'y'
        status = 'accepted' if answer in ('y', 'yes') else 'rejected'
        if status == 'accepted':
            userdata['accepted_update_id'] = payload.get('update_id')
        else:
            userdata['accepted_update_id'] = None

        response = {
            'type': 'update_response',
            'update_id': payload.get('update_id'),
            'requester_id': userdata['gateway_id'],
            'status': status,
        }
        client.publish(RESPONSE_TOPIC, json.dumps(response), qos=1)
        print('Response sent:', status)
        return

    if payload.get('recipient_id') != userdata['gateway_id']:
        return

    if payload.get('update_id') != userdata.get('accepted_update_id'):
        print('Ignoring a file that was not accepted.')
        return

    try:
        file_bytes = base64.b64decode(payload['data'], validate=True)
        filename = os.path.basename(payload.get('filename', 'update.bin'))
        os.makedirs(userdata['output_dir'], exist_ok=True)
        output_path = os.path.join(userdata['output_dir'], filename)
        with open(output_path, 'wb') as output_file:
            output_file.write(file_bytes)
        print('Accepted update file saved:', output_path)
    except (KeyError, ValueError) as exc:
        print('File save failed:', exc)


def main():
    print('=== Practice 2 Central Gateway ===')
    broker = input('Broker IP [localhost]: ').strip() or 'localhost'
    port = int(input('Broker Port [1883]: ').strip() or '1883')
    gateway_id = input('Central Gateway ID: ').strip()
    output_dir = input('Download Directory [received_updates]: ').strip() or 'received_updates'
    username = input('MQTT Username (anonymous이면 Enter): ').strip()
    password = getpass.getpass('MQTT Password: ') if username else None

    if not gateway_id:
        print('Central Gateway ID is required.')
        return

    userdata = {
        'gateway_id': gateway_id,
        'output_dir': output_dir,
        'accepted_update_id': None,
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
