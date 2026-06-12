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
    print('Published payload, MID:', mid)


def build_payload(topic, payload_type, file_path=None):
    if payload_type == 'file':
        with open(file_path, 'rb') as f:
            file_bytes = f.read()
        return json.dumps({
            'type': 'file',
            'topic': topic,
            'filename': os.path.basename(file_path),
            'data': base64.b64encode(file_bytes).decode('utf-8'),
        })


def main():
    print('=== Practice 3 Publisher Solution ===')
    broker_ip = input('Broker IP [localhost]: ').strip() or 'localhost'
    port_text = input('Broker Port [1883]: ').strip() or '1883'
    try:
        port = int(port_text)
    except ValueError:
        print('Port must be a number. Using 1883.')
        port = 1883     
    topic = input('Publish Topic: ').strip()
    if not topic:
        print('Publish topic is required.')
        return

    payload_type = input('Payload type (message/file) [message]: ').strip().lower() or 'message'
    if payload_type not in ('message', 'file'):
        print("Payload type must be 'message' or 'file'.")
        return

    if payload_type == 'message':
        message = input('Message to publish [Hello! I am an update server.]: ').strip() or 'Hello! I am an update server.'

    elif payload_type == 'file':
        file_path = input('File path to send: ').strip()
        if not os.path.isfile(file_path):
            print('File not found:', file_path)
            return
    
    if payload_type == 'message':
        payload = message.encode('utf-8')
        topic = f'{topic}/message'

    elif payload_type == 'file':
        payload = build_payload(topic, payload_type, file_path)
        topic = f'{topic}/file'

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
        print(f'Sent {payload_type} payload to topic: {topic}')
    except Exception as exc:
        print('Publish error:', exc)


if __name__ == '__main__':
    main()
