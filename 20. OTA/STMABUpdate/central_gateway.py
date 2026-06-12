"""MQTT-to-CAN Central Gateway for the STM32 A/B update example."""

from __future__ import annotations

import argparse
import base64
import getpass
import hashlib
import json
import os
import struct
import subprocess
import threading
import time
import uuid
from typing import Any

NOTICE_TOPIC = "stmab/update/notice"
RESPONSE_TOPIC = "stmab/update/response"
PACKAGE_TOPIC = "stmab/update/package"
RESULT_TOPIC = "stmab/update/result"
PARTITION_SIZE = 48 * 1024
CAN_ID_FILE = 0x70
CAN_ID_SIZE = 0x71
CAN_ID_SEND = 0x7A
CAN_ID_CONTROL = 0x7B

UPDATE_COMMAND = bytes((0xFF, 0x00, 0xFF, 0x00, 0xFF, 0x00, 0xFF, 0x00))
APPLICATION_A_ADDRESS = 0x08008000
APPLICATION_B_ADDRESS = 0x08014000


def load_mqtt():
    try:
        import paho.mqtt.client as mqtt
    except ImportError as exc:
        raise RuntimeError("paho-mqtt is required: pip install paho-mqtt") from exc
    if not hasattr(mqtt, "Client") or not hasattr(mqtt, "CallbackAPIVersion"):
        raise RuntimeError("paho-mqtt 2.x is required: pip install -U paho-mqtt")
    return mqtt


def load_can():
    try:
        import can
    except ImportError as exc:
        raise RuntimeError("python-can is required: pip install python-can") from exc
    if not hasattr(can, "Message") or not hasattr(can, "interface"):
        raise RuntimeError(
            "The imported 'can' package is not python-can. "
            "Install the correct package: pip install python-can"
        )
    return can


def parse_version(version: str) -> tuple[int, ...]:
    parts = version.split(".")
    if not parts or any(not part.isdigit() for part in parts):
        raise ValueError(f"invalid version: {version}")
    return tuple(int(part) for part in parts)


def is_newer(candidate: str, current: str) -> bool:
    candidate_parts = parse_version(candidate)
    current_parts = parse_version(current)
    width = max(len(candidate_parts), len(current_parts))
    return (
        candidate_parts + (0,) * (width - len(candidate_parts))
        > current_parts + (0,) * (width - len(current_parts))
    )


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


def configure_socketcan(channel: str, bitrate: int) -> None:
    prefix = [] if hasattr(os, "geteuid") and os.geteuid() == 0 else ["sudo"]
    commands = (
        ["ip", "link", "set", channel, "down"],
        ["ip", "link", "set", channel, "type", "can", "bitrate", str(bitrate)],
        ["ip", "link", "set", channel, "up"],
    )
    for command in commands:
        result = subprocess.run(
            prefix + command,
            check=False,
            capture_output=True,
            text=True,
        )
        if result.returncode != 0 and command[-1] != "down":
            detail = result.stderr.strip() or result.stdout.strip()
            raise RuntimeError(f"SocketCAN setup failed: {detail}")


class CentralGateway:
    def __init__(self, args: argparse.Namespace) -> None:
        self.args = args
        self.notice: dict[str, Any] | None = None
        self.current_version = args.current_version
        self.can_bus: Any = None
        self.client: Any = None
        self.update_lock = threading.Lock()

    def open_can(self) -> None:
        can = load_can()
        if self.args.configure_can:
            if self.args.can_interface != "socketcan":
                raise ValueError("--configure-can is only valid with socketcan")
            configure_socketcan(self.args.can_channel, self.args.can_bitrate)

        kwargs: dict[str, Any] = {
            "interface": self.args.can_interface,
            "channel": self.args.can_channel,
        }
        if self.args.can_interface != "socketcan":
            kwargs["bitrate"] = self.args.can_bitrate
        self.can_bus = can.interface.Bus(**kwargs)
        print(
            f"[GATEWAY] CAN open: {self.args.can_interface}/"
            f"{self.args.can_channel}, bitrate={self.args.can_bitrate}"
        )

    def send_can(self, arbitration_id: int, data: bytes) -> None:
        can = load_can()
        self.can_bus.send(
            can.Message(
                arbitration_id=arbitration_id,
                data=data,
                is_extended_id=False,
            ),
            timeout=1.0,
        )

    def receive_status(self, timeout: float, expected_length: int) -> bytes:
        deadline = time.monotonic() + timeout
        while time.monotonic() < deadline:
            message = self.can_bus.recv(timeout=min(0.25, deadline - time.monotonic()))
            if message is None or message.arbitration_id != CAN_ID_SEND:
                continue
            data = bytes(message.data)
            if len(data) == expected_length:
                return data
        raise TimeoutError(
            f"STM32 response timeout: CAN ID=0x{CAN_ID_SEND:X}, "
            f"DLC={expected_length}"
        )

    @staticmethod
    def decode_image(package: dict[str, Any], key: str) -> bytes:
        metadata = package[key]
        data = base64.b64decode(metadata["data"], validate=True)
        if len(data) != int(metadata["size"]):
            raise ValueError(f"{key} size mismatch")
        if sha256(data) != metadata["sha256"]:
            raise ValueError(f"{key} SHA-256 mismatch")
        return data

    def flash_stm32(self, image_a: bytes, image_b: bytes) -> str:
        transfer_size = max(len(image_a), len(image_b))
        transfer_size += transfer_size % 2
        if transfer_size > PARTITION_SIZE:
            raise ValueError(f"transfer exceeds partition size: {transfer_size}")

        self.send_can(CAN_ID_CONTROL, UPDATE_COMMAND)
        self.send_can(CAN_ID_SIZE, struct.pack(">I", transfer_size))
        print(f"[GATEWAY] Update requested: {transfer_size} bytes")

        address_data = self.receive_status(self.args.address_timeout, 4)
        target_address = struct.unpack(">I", address_data)[0]
        if target_address == APPLICATION_A_ADDRESS:
            partition = "A"
            image = image_a
        elif target_address == APPLICATION_B_ADDRESS:
            partition = "B"
            image = image_b
        else:
            raise ValueError(f"unexpected target address: 0x{target_address:08X}")

        padded_image = image.ljust(transfer_size, b"\xFF")
        print(
            f"[GATEWAY] STM32 selected partition {partition} "
            f"(0x{target_address:08X})"
        )
        for offset in range(0, transfer_size, 8):
            self.send_can(CAN_ID_FILE, padded_image[offset : offset + 8])
            if self.args.frame_delay:
                time.sleep(self.args.frame_delay)
            if offset == 0 or (offset + 8) % 1024 == 0:
                sent = min(offset + 8, transfer_size)
                print(f"\r[GATEWAY] Sending {sent}/{transfer_size}", end="", flush=True)
        print()

        result = self.receive_status(self.args.result_timeout, 1)[0]
        if result == 0x02:
            print("[GATEWAY] STM32 reported update success")
            return partition
        if result == 0x03:
            raise RuntimeError("STM32 reported update failure")
        raise RuntimeError(f"unknown STM32 status: 0x{result:02X}")

    def publish_result(
        self, update_id: str | None, status: str, reason: str = "", partition: str = ""
    ) -> None:
        publish_json(
            self.client,
            RESULT_TOPIC,
            {
                "type": "stm_ab_update_result",
                "update_id": update_id,
                "status": status,
                "reason": reason,
                "partition": partition,
            },
        )

    def on_connect(
        self, client: Any, userdata: Any, flags: Any, reason_code: Any, properties: Any
    ) -> None:
        if reason_code != 0:
            print(f"[GATEWAY] MQTT connection failed: {reason_code}")
            return
        client.subscribe(NOTICE_TOPIC, qos=1)
        client.subscribe(PACKAGE_TOPIC, qos=1)
        print("[GATEWAY] Waiting for STM32 A/B update")

    def on_message(self, client: Any, userdata: Any, message: Any) -> None:
        try:
            payload = json.loads(message.payload.decode("utf-8"))
        except (UnicodeDecodeError, json.JSONDecodeError) as exc:
            print(f"[GATEWAY] Invalid MQTT payload: {exc}")
            return

        if message.topic == NOTICE_TOPIC:
            try:
                if not is_newer(payload["version"], self.current_version):
                    raise ValueError("same or lower version")
                if int(payload["transfer_size"]) > PARTITION_SIZE:
                    raise ValueError("image exceeds the 48 KiB partition")
                for key in ("partition_a", "partition_b"):
                    if int(payload[key]["size"]) <= 0:
                        raise ValueError(f"{key} is empty")
            except (KeyError, TypeError, ValueError) as exc:
                publish_json(
                    client,
                    RESPONSE_TOPIC,
                    {
                        "update_id": payload.get("update_id"),
                        "status": "ignored",
                        "reason": str(exc),
                    },
                )
                print(f"[GATEWAY] Notice ignored: {exc}")
                return

            self.notice = payload
            publish_json(
                client,
                RESPONSE_TOPIC,
                {"update_id": payload["update_id"], "status": "accepted"},
            )
            print(
                f"[GATEWAY] Update accepted: version={payload['version']}, "
                f"id={payload['update_id']}"
            )
            return

        if not self.notice or payload.get("update_id") != self.notice.get("update_id"):
            return
        if not self.update_lock.acquire(blocking=False):
            print("[GATEWAY] Update already in progress; duplicate package ignored")
            return

        update_id = payload.get("update_id")
        try:
            image_a = self.decode_image(payload, "partition_a")
            image_b = self.decode_image(payload, "partition_b")
            partition = self.flash_stm32(image_a, image_b)
            self.current_version = payload["version"]
            self.notice = None
            self.publish_result(update_id, "success", partition=partition)
        except Exception as exc:
            print(f"[GATEWAY] Update failed: {exc}")
            self.publish_result(update_id, "failed", reason=str(exc))
        finally:
            self.update_lock.release()

    def run(self) -> None:
        parse_version(self.current_version)
        self.open_can()
        mqtt = load_mqtt()
        self.client = create_mqtt_client(
            mqtt, f"stmab-gateway-{uuid.uuid4().hex[:8]}", self
        )
        self.client.on_connect = self.on_connect
        self.client.on_message = self.on_message
        apply_mqtt_credentials(self.client, self.args)
        try:
            self.client.connect(self.args.broker, self.args.port, self.args.keepalive)
            self.client.loop_forever()
        except KeyboardInterrupt:
            print("\n[GATEWAY] Stopped")
        finally:
            self.client.disconnect()
            if self.can_bus is not None:
                self.can_bus.shutdown()


def build_parser() -> argparse.ArgumentParser:
    parser = argparse.ArgumentParser(description="STM32 A/B Central Gateway")
    add_mqtt_arguments(parser)
    parser.add_argument("--current-version", default="1.0.0")
    parser.add_argument("--can-interface", default="socketcan")
    parser.add_argument("--can-channel", default="can0")
    parser.add_argument("--can-bitrate", type=int, default=1_000_000)
    parser.add_argument("--configure-can", action="store_true")
    parser.add_argument("--frame-delay", type=float, default=0.001)
    parser.add_argument("--address-timeout", type=float, default=10.0)
    parser.add_argument("--result-timeout", type=float, default=30.0)
    return parser


def main() -> None:
    CentralGateway(build_parser().parse_args()).run()


if __name__ == "__main__":
    main()
