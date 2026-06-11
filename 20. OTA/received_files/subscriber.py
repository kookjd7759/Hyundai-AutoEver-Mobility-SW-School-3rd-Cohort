import paho.mqtt.client as mqtt
import json
import os
import base64

TOPIC = "topic"
BROKER_IP = "127.0.0.1"
BROKER_PORT = 1883

SAVE_DIR = "./received"

os.makedirs(SAVE_DIR, exist_ok=True)


def on_connect(client, userdata, flags, reasonCode):
    if reasonCode == 0:
        print("Connected successfully.")
        client.subscribe(TOPIC)
    else:
        print(f"Failed to connect, return code {reasonCode}")


def on_disconnect(client, userdata, flags, rc=0):
    print(f"Disconnected: {rc}")


def on_message(client, userdata, msg):

    try:
        payload = json.loads(msg.payload)

        if payload["type"] == "msg":

            print("\n[TEXT MESSAGE]")
            print(payload["message"])

        elif payload["type"] == "file":

            filename = payload["filename"]

            file_data = base64.b64decode(
                payload["data"]
            )

            filepath = os.path.join(
                SAVE_DIR,
                filename
            )

            with open(filepath, "wb") as f:
                f.write(file_data)

            print(f"\n[FILE RECEIVED]")
            print(f"파일명 : {filename}")
            print(f"저장 위치 : {filepath}")
            print(f"파일 크기 : {os.path.getsize(filepath)} bytes")

        else:
            print("알 수 없는 타입")

    except Exception as e:
        print(f"Error: {e}")


def topic_subscribe():

    client = mqtt.Client()

    client.on_connect = on_connect
    client.on_disconnect = on_disconnect
    client.on_message = on_message

    try:
        client.connect(
            BROKER_IP,
            BROKER_PORT,
            keepalive=60
        )

        client.loop_forever()

    except Exception as e:
        print(f"Connection error: {e}")


if __name__ == "__main__":
    topic_subscribe()