import base64
import json
import os

import paho.mqtt.client as mqtt


def on_connect(client, userdata, flags, reasonCode):
    if reasonCode == 0:
        print('Connected successfully.')
        client.subscribe(userdata['topic'])
        print('Subscribed to topic:', userdata['topic'])
    else:
        print('Connection failed, reason code:', reasonCode)


def on_disconnect(client, userdata, rc):
    print('Disconnected, RC:', rc)


def save_file(output_dir, filename, data_bytes):
    os.makedirs(output_dir, exist_ok=True)
    safe_filename = os.path.basename(filename) or 'received_file'
    path = os.path.join(output_dir, safe_filename)
    with open(path, 'wb') as f:
        f.write(data_bytes)
    print('Saved file to:', path)


def on_message(client, userdata, msg):
    try:
        text = msg.payload.decode('utf-8')
        payload = json.loads(text)
    except Exception:
        payload = None

    if isinstance(payload, dict) and payload.get('type') == 'file':
        try:
            data = base64.b64decode(payload.get('data', ''), validate=True)
            filename = payload.get('filename', 'received_file')
            save_file(userdata['output_dir'], filename, data)
        except Exception as exc:
            print('Failed to save file:', exc)
    else:
        print('Received non-file payload:', msg.payload)


def main():
    print('=== Practice 2 Subscriber Solution ===')
    broker_ip = input('Broker IP [localhost]: ').strip() or 'localhost'
    port_text = input('Broker Port [1883]: ').strip() or '1883'
    topic = input('Subscribe Topic: ').strip()
    if not topic:
        print('Subscribe topic is required.')
        return

    output_dir = input('Output directory for files [received_files]: ').strip() or 'received_files'

    try:
        port = int(port_text)
    except ValueError:
        print('Port must be a number. Using 1883.')
        port = 1883

    client = mqtt.Client(userdata={'topic': topic, 'output_dir': output_dir})
    client.on_connect = on_connect
    client.on_disconnect = on_disconnect
    client.on_message = on_message

    try:
        client.connect(broker_ip, port, keepalive=60)
        client.loop_forever()
    except Exception as exc:
        print('Subscribe error:', exc)


if __name__ == '__main__':
    main()
