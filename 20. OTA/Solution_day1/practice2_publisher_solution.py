import base64
import json
import os

import paho.mqtt.client as mqtt


def on_connect(client, userdata, flags, reasonCode):
    if reasonCode == 0:
        print('Connected OK')
    else:
        print('Connection failed, reason code:', reasonCode)


def on_disconnect(client, userdata, rc):
    print('Disconnected, RC:', rc)


def on_publish(client, userdata, mid):
    print('Published file message, MID:', mid)


def build_payload(topic, file_path):
    with open(file_path, 'rb') as f:
        file_bytes = f.read()
    payload = {
        'type': 'file',
        'topic': topic,
        'filename': os.path.basename(file_path),
        'data': base64.b64encode(file_bytes).decode('utf-8'),
    }
    return json.dumps(payload)


def main():
    print('=== Practice 2 Publisher Solution ===')
    broker_ip = input('Broker IP [localhost]: ').strip() or 'localhost'
    port_text = input('Broker Port [1883]: ').strip() or '1883'
    topic = input('Publish Topic: ').strip()
    if not topic:
        print('Publish topic is required.')
        return

    file_path = input('File path to send: ').strip()

    if not os.path.isfile(file_path):
        print('File not found:', file_path)
        return

    try:
        port = int(port_text)
    except ValueError:
        print('Port must be a number. Using 1883.')
        port = 1883

    payload = build_payload(topic, file_path)

    client = mqtt.Client()
    client.on_connect = on_connect
    client.on_disconnect = on_disconnect
    client.on_publish = on_publish

    try:
        client.connect(broker_ip, port)
        client.loop_start()
        publish_info = client.publish(topic, payload, qos=1)
        publish_info.wait_for_publish()
        client.loop_stop()
        client.disconnect()
        print(f'Sent file {file_path} to topic: {topic}')
    except Exception as exc:
        print('Publish error:', exc)


if __name__ == '__main__':
    main()
