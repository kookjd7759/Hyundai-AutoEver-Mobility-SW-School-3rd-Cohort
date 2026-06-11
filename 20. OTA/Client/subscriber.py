import paho.mqtt.client as mqtt
from pathlib import Path
import json
import base64
import hashlib

TOPIC = 'topic'
BROKER_IP = "192.168.203.50"
BROKER_PORT = 1883

SAVE_DIR = Path("received_files")
SAVE_DIR.mkdir(exist_ok=True)

def on_connect(client, userdata, flags, reasonCode):
    if reasonCode == 0:
        print("Connected successfully.")
        client.subscribe(TOPIC, qos=2)
        print("Subscribed:", TOPIC)
    else:
        print(f"Failed to connect, return code {reasonCode}")

def on_disconnect(client, userdata, flags, rc=0):
    print("Disconnected:", rc)

# 텍스트 메시지 처리
def on_message_msg(data):
    message = data["message"]
    print("Receive text message:", message)

# 파일 처리
def on_message_file(data):
    filename = Path(data["filename"]).name
    original_size = data["size"]
    original_hash = data["sha256"]

    file_data = base64.b64decode(data["data"])

    save_path = SAVE_DIR / filename
    save_path.write_bytes(file_data)

    saved_size = save_path.stat().st_size
    saved_hash = hashlib.sha256(save_path.read_bytes()).hexdigest()

    print("File received and saved.")
    print("Saved path:", save_path)
    print("File name:", filename)
    print("Original size:", original_size, "bytes")
    print("Saved size:", saved_size, "bytes")

    if original_size == saved_size and original_hash == saved_hash:
        print("Verify result: original file and saved file are identical.")
    else:
        print("Verify result: file is different.")

# 수신 Payload 분석 후 분기
def on_message(client, userdata, msg):
    try:
        payload = msg.payload.decode("utf-8")
        data = json.loads(payload)

        payload_type = data.get("type")

        if payload_type == "msg":
            on_message_msg(data)

        elif payload_type == "file":
            on_message_file(data)

        else:
            print("Unknown payload type:", payload_type)

    except Exception as e:
        print(f"Error: {e}")


def topic_subscribe():
    client = mqtt.Client()
    client.on_connect = on_connect
    client.on_disconnect = on_disconnect
    client.on_message = on_message

    try:
        client.connect(BROKER_IP, BROKER_PORT, keepalive=60)
        client.loop_forever()

    except Exception as e:
        print(f"Connection error: {e}")


if __name__ == "__main__":
    topic_subscribe()