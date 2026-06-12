"""MQTT Update Server for the STM32 A/B update example."""

from __future__ import annotations

import argparse
import base64
import getpass
import hashlib
import json
import threading
import uuid
from pathlib import Path
from typing import Any


NOTICE_TOPIC = "stmab/update/notice"
RESPONSE_TOPIC = "stmab/update/response"
PACKAGE_TOPIC = "stmab/update/package"
RESULT_TOPIC = "stmab/update/result"
PARTITION_SIZE = 48 * 1024


def load_mqtt():
    try:
        import paho.mqtt.client as mqtt
    except ImportError as exc:
        raise RuntimeError("paho-mqtt is required: pip install paho-mqtt") from exc
    if not hasattr(mqtt, "Client") or not hasattr(mqtt, "CallbackAPIVersion"):
        raise RuntimeError("paho-mqtt 2.x is required: pip install -U paho-mqtt")
    return mqtt


def parse_version(version: str) -> tuple[int, ...]:
    parts = version.split(".")
    if not parts or any(not part.isdigit() for part in parts):
        raise ValueError(f"invalid version: {version}")
    return tuple(int(part) for part in parts)


def sha256(data: bytes) -> str:
    return hashlib.sha256(data).hexdigest()


def publish_json(client: Any, topic: str, payload: dict[str, Any]) -> None:
    info = client.publish(topic, json.dumps(payload), qos=1)
    if info.rc != 0:
        raise RuntimeError(f"MQTT publish failed: topic={topic}, rc={info.rc}")


def create_mqtt_client(mqtt: Any, client_id: str, userdata: Any) -> Any:
    return mqtt.Client(
        callback_api_version=mqtt.CallbackAPIVersion.VERSION2,
        client_id=client_id,
        userdata=userdata,
    )


def add_mqtt_arguments(parser: argparse.ArgumentParser) -> None:
    parser.add_argument("--broker", default="localhost")
    parser.add_argument("--port", type=int, default=1883)
    parser.add_argument("--keepalive", type=int, default=60)
    parser.add_argument("--username", default="")
    parser.add_argument("--password")


def apply_mqtt_credentials(client: Any, args: argparse.Namespace) -> None:
    if args.username:
        password = args.password
        if password is None:
            password = getpass.getpass("MQTT password: ")
        client.username_pw_set(args.username, password)


def read_image(path: str, slot: str) -> bytes:
    image_path = Path(path)
    if not image_path.is_file():
        raise FileNotFoundError(f"Partition {slot} image not found: {image_path}")
    data = image_path.read_bytes()
    if not data:
        raise ValueError(f"Partition {slot} image is empty")
    if len(data) > PARTITION_SIZE:
        raise ValueError(
            f"Partition {slot} image is too large: {len(data)} > {PARTITION_SIZE}"
        )
    return data


def image_metadata(path: str, data: bytes) -> dict[str, Any]:
    return {
        "filename": Path(path).name,
        "size": len(data),
        "sha256": sha256(data),
    }


class UpdateServer:
    def __init__(self, args: argparse.Namespace) -> None:
        self.args = args
        self.image_a = read_image(args.partition_a, "A")
        self.image_b = read_image(args.partition_b, "B")
        self.finished = threading.Event()
        self.retry_count = 0
        self.update_id = uuid.uuid4().hex[:12]
        self.client: Any = None

    def notice(self) -> dict[str, Any]:
        transfer_size = max(len(self.image_a), len(self.image_b))
        transfer_size += transfer_size % 2
        return {
            "type": "stm_ab_update_notice",
            "update_id": self.update_id,
            "version": self.args.version,
            "transfer_size": transfer_size,
            "partition_a": image_metadata(self.args.partition_a, self.image_a),
            "partition_b": image_metadata(self.args.partition_b, self.image_b),
        }

    def package(self) -> dict[str, Any]:
        payload = self.notice()
        payload["type"] = "stm_ab_update_package"
        payload["partition_a"]["data"] = base64.b64encode(self.image_a).decode("ascii")
        payload["partition_b"]["data"] = base64.b64encode(self.image_b).decode("ascii")
        return payload

    def on_connect(
        self, client: Any, userdata: Any, flags: Any, reason_code: Any, properties: Any
    ) -> None:
        if reason_code != 0:
            print(f"[SERVER] MQTT connection failed: {reason_code}")
            self.finished.set()
            return
        client.subscribe(RESPONSE_TOPIC, qos=1)
        client.subscribe(RESULT_TOPIC, qos=1)
        publish_json(client, NOTICE_TOPIC, self.notice())
        print(f"[SERVER] Notice sent: update_id={self.update_id}")

    def on_message(self, client: Any, userdata: Any, message: Any) -> None:
        try:
            payload = json.loads(message.payload.decode("utf-8"))
        except (UnicodeDecodeError, json.JSONDecodeError):
            return
        if payload.get("update_id") != self.update_id:
            return

        if message.topic == RESPONSE_TOPIC:
            if payload.get("status") == "accepted":
                publish_json(client, PACKAGE_TOPIC, self.package())
                print("[SERVER] Gateway accepted; A/B package sent")
            elif payload.get("status") == "ignored":
                print(f"[SERVER] Update ignored: {payload.get('reason', '')}")
                self.finished.set()
            return

        if payload.get("status") == "success":
            print(
                "[SERVER] STM32 update completed: "
                f"partition={payload.get('partition')}"
            )
            self.finished.set()
        elif payload.get("status") == "failed":
            reason = payload.get("reason", "")
            if self.retry_count < self.args.max_retries:
                self.retry_count += 1
                print(
                    f"[SERVER] Update failed ({reason}); "
                    f"retry {self.retry_count}/{self.args.max_retries}"
                )
                publish_json(client, PACKAGE_TOPIC, self.package())
            else:
                print(f"[SERVER] Update failed: {reason}")
                self.finished.set()

    def run(self) -> None:
        mqtt = load_mqtt()
        self.client = create_mqtt_client(mqtt, f"stmab-server-{self.update_id}", self)
        self.client.on_connect = self.on_connect
        self.client.on_message = self.on_message
        apply_mqtt_credentials(self.client, self.args)
        self.client.connect(self.args.broker, self.args.port, self.args.keepalive)
        self.client.loop_start()
        try:
            self.finished.wait()
        except KeyboardInterrupt:
            print("\n[SERVER] Stopped")
        finally:
            self.client.loop_stop()
            self.client.disconnect()


def build_parser() -> argparse.ArgumentParser:
    parser = argparse.ArgumentParser(description="STM32 A/B MQTT Update Server")
    add_mqtt_arguments(parser)
    parser.add_argument("--partition-a", required=True, help="A-linked raw .bin")
    parser.add_argument("--partition-b", required=True, help="B-linked raw .bin")
    parser.add_argument("--version", default="1.1.0")
    parser.add_argument("--max-retries", type=int, default=2)
    return parser


def main() -> None:
    args = build_parser().parse_args()
    parse_version(args.version)
    UpdateServer(args).run()


if __name__ == "__main__":
    main()
