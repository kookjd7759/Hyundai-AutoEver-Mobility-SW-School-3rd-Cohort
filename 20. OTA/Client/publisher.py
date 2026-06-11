import paho.mqtt.client as mqtt
from pathlib import Path
import json
import base64
import hashlib
import getpass
import threading

BROKER_PORT = 1883


def on_connect(client, userdata, flags, reasonCode):
    if reasonCode == 0:
        print("connected OK")
        userdata["connected"] = True
    else:
        print("Error: connection failed, Return Code =", reasonCode)
        userdata["connected"] = False

    userdata["connect_event"].set()


def on_disconnect(client, userdata, flags, rc=0):
    print("Disconnected, RC:", rc)


def on_publish(client, userdata, mid):
    print("message published, MID:", mid)


def make_client(mqtt_id, mqtt_password):
    state = {
        "connected": False,
        "connect_event": threading.Event()
    }

    client = mqtt.Client(userdata=state)
    client.on_connect = on_connect
    client.on_disconnect = on_disconnect
    client.on_publish = on_publish

    client.username_pw_set(mqtt_id, mqtt_password)

    return client, state


def publish_payload(topic, payload, broker_ip, mqtt_id, mqtt_password, port=1883):
    client, state = make_client(mqtt_id, mqtt_password)

    try:
        client.connect(broker_ip, port)
        client.loop_start()

        # 연결 결과 대기
        connected = state["connect_event"].wait(timeout=5)

        if not connected:
            print("Error: broker connection timeout")
            client.loop_stop()
            client.disconnect()
            return False

        if not state["connected"]:
            print("Error: authentication failed or connection rejected")
            client.loop_stop()
            client.disconnect()
            return False

        result = client.publish(topic, payload, qos=2)
        result.wait_for_publish()

        client.loop_stop()
        client.disconnect()

        return True

    except Exception as e:
        print("Error:", e)
        return False


# 텍스트 메시지 전송
def message_publish_msg(topic, message, broker_ip, mqtt_id, mqtt_password, port=1883):
    data = {
        "type": "msg",
        "message": message
    }

    payload = json.dumps(data, ensure_ascii=False).encode("utf-8")

    success = publish_payload(topic, payload, broker_ip, mqtt_id, mqtt_password, port)

    if success:
        print(f"Success sending text message: {message}")
    else:
        print("Failed sending text message")


# 파일 전송
def message_publish_file(topic, file_path, broker_ip, mqtt_id, mqtt_password, port=1883):
    path = Path(file_path)

    if not path.exists():
        print("Error: file does not exist:", file_path)
        return

    file_data = path.read_bytes()

    data = {
        "type": "file",
        "filename": path.name,
        "size": len(file_data),
        "sha256": hashlib.sha256(file_data).hexdigest(),
        "data": base64.b64encode(file_data).decode("utf-8")
    }

    payload = json.dumps(data, ensure_ascii=False).encode("utf-8")

    success = publish_payload(topic, payload, broker_ip, mqtt_id, mqtt_password, port)

    if success:
        print(f"Success sending file: {path.name}")
        print(f"File size: {len(file_data)} bytes")
    else:
        print("Failed sending file")

DEBUG = True

if __name__ == '__main__':
    if DEBUG:
        broker_ip = '192.168.203.50'
        topic = 'topic'
    else:
        broker_ip = input("Broker IP 입력: ").strip()
        topic = input("Topic 입력: ").strip()

    if DEBUG:
        mqtt_id = 'ecu1'
        mqtt_password = '1234'
    else:
        mqtt_id = input("MQTT 사용자 이름 입력: ").strip()
        mqtt_password = getpass.getpass("MQTT 비밀번호 입력: ")

    print()
    print("1. 텍스트 메시지 전송")
    print("2. 파일 전송")

    select = input("전송 방식 선택: ").strip()

    if select == "1":
        message = input("전송할 메시지 입력: ")
        message_publish_msg(
            topic,
            message,
            broker_ip,
            mqtt_id,
            mqtt_password,
            BROKER_PORT
        )

    elif select == "2":
        file_path = input("전송할 파일 경로 입력: ").strip().strip('"')
        message_publish_file(
            topic,
            file_path,
            broker_ip,
            mqtt_id,
            mqtt_password,
            BROKER_PORT
        )

    else:
        print("잘못된 선택입니다.")