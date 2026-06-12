import base64
import getpass
import hashlib
import json
import os

import paho.mqtt.client as mqtt


DELTA_TOPIC = 'ota/challenge/delta'


def apply_delta(old_bytes, payload):
    if hashlib.sha256(old_bytes).hexdigest() != payload['old_sha256']:
        raise ValueError('The local base file does not match the delta base.')

    block_size = int(payload['block_size'])
    new_size = int(payload['new_size'])
    result = bytearray(old_bytes)
    if len(result) < new_size:
        result.extend(b'\x00' * (new_size - len(result)))

    for block in payload['blocks']:
        index = int(block['index'])
        data = base64.b64decode(block['data'], validate=True)
        start = index * block_size
        result[start:start + len(data)] = data

    result = bytes(result[:new_size])
    if hashlib.sha256(result).hexdigest() != payload['new_sha256']:
        raise ValueError('Delta result SHA-256 mismatch.')
    return result


def on_connect(client, userdata, flags, reason_code):
    if reason_code == 0:
        client.subscribe(DELTA_TOPIC, qos=1)
        print('Waiting for a delta update...')


def on_message(client, userdata, message):
    try:
        payload = json.loads(message.payload.decode('utf-8'))
        with open(userdata['base_path'], 'rb') as base_file:
            old_bytes = base_file.read()
        new_bytes = apply_delta(old_bytes, payload)
        os.makedirs(userdata['output_dir'], exist_ok=True)
        output_path = os.path.join(
            userdata['output_dir'],
            os.path.basename(payload.get('filename', 'updated.bin')),
        )
        with open(output_path, 'wb') as output_file:
            output_file.write(new_bytes)
        print('Delta update applied:', output_path)
    except Exception as exc:
        print('Delta update failed:', exc)


def main():
    print('=== Challenge 2 Central Gateway: Delta Apply ===')
    broker = input('Broker IP [localhost]: ').strip() or 'localhost'
    port = int(input('Broker Port [1883]: ').strip() or '1883')
    base_path = input('Installed Base File Path: ').strip()
    output_dir = input('Output Directory [delta_updates]: ').strip() or 'delta_updates'
    username = input('MQTT Username (anonymous이면 Enter): ').strip()
    password = getpass.getpass('MQTT Password: ') if username else None

    if not os.path.isfile(base_path):
        print('Base file not found:', base_path)
        return
    userdata = {'base_path': base_path, 'output_dir': output_dir}
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
