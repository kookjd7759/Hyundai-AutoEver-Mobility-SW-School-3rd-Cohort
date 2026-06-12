import base64
import getpass
import hashlib
import json
import os

import paho.mqtt.client as mqtt


DELTA_TOPIC = 'ota/challenge/delta'
BLOCK_SIZE = 64


def build_delta(old_bytes, new_bytes):
    changed_blocks = []
    block_count = (len(new_bytes) + BLOCK_SIZE - 1) // BLOCK_SIZE
    for index in range(block_count):
        start = index * BLOCK_SIZE
        new_block = new_bytes[start:start + BLOCK_SIZE]
        old_block = old_bytes[start:start + BLOCK_SIZE]
        if new_block != old_block:
            changed_blocks.append({
                'index': index,
                'data': base64.b64encode(new_block).decode('ascii'),
            })
    return changed_blocks


def main():
    print('=== Challenge 2 Update Server: Delta Update ===')
    broker = input('Broker IP [localhost]: ').strip() or 'localhost'
    port = int(input('Broker Port [1883]: ').strip() or '1883')
    old_path = input('Old Version File Path: ').strip()
    new_path = input('New Version File Path: ').strip()
    username = input('MQTT Username (anonymous이면 Enter): ').strip()
    password = getpass.getpass('MQTT Password: ') if username else None

    if not os.path.isfile(old_path) or not os.path.isfile(new_path):
        print('Old or new version file does not exist.')
        return
    with open(old_path, 'rb') as old_file:
        old_bytes = old_file.read()
    with open(new_path, 'rb') as new_file:
        new_bytes = new_file.read()

    changed_blocks = build_delta(old_bytes, new_bytes)
    payload = {
        'type': 'delta_update',
        'filename': os.path.basename(new_path),
        'block_size': BLOCK_SIZE,
        'new_size': len(new_bytes),
        'old_sha256': hashlib.sha256(old_bytes).hexdigest(),
        'new_sha256': hashlib.sha256(new_bytes).hexdigest(),
        'blocks': changed_blocks,
    }
    encoded_payload = json.dumps(payload).encode('utf-8')

    client = mqtt.Client()
    if username:
        client.username_pw_set(username, password)
    client.connect(broker, port)
    client.loop_start()
    try:
        info = client.publish(DELTA_TOPIC, encoded_payload, qos=1)
        info.wait_for_publish()
        print(f'Full file size: {len(new_bytes)} bytes')
        print(f'Delta message size: {len(encoded_payload)} bytes')
        print(f'Changed blocks: {len(changed_blocks)}')
    finally:
        client.loop_stop()
        client.disconnect()


if __name__ == '__main__':
    main()
