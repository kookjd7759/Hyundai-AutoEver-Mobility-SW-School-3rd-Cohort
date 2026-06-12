import os
import struct
import subprocess
import sys
import time
from pathlib import Path


BASE_DIR = Path(__file__).resolve().parent
FLAG_FILE = BASE_DIR / "active_slot.txt"
SLOTS_DIR = BASE_DIR / "slots"

ECU_NAME = "powertrain"
UPDATE_FILENAME = "cluster.py"
CAN_CHANNEL = "can0"
CAN_BITRATE = 1000000
STARTUP_CHECK_SECONDS = 3

# START, DATA, END, RESULT CAN ID
ECU_CAN_ROUTES = {
    "powertrain": (0x700, 0x701, 0x702, 0x703),
    "body": (0x710, 0x711, 0x712, 0x713),
    "adas": (0x720, 0x721, 0x722, 0x723),
}


def slot_directory(slot: str) -> Path:
    if slot not in {"A", "B"}:
        raise ValueError("Slot은 A 또는 B여야 합니다.")
    return SLOTS_DIR / f"slot_{slot.lower()}"


def prepare_slots() -> None:
    slot_directory("A").mkdir(parents=True, exist_ok=True)
    slot_directory("B").mkdir(parents=True, exist_ok=True)
    if not FLAG_FILE.exists():
        FLAG_FILE.write_text("A", encoding="utf-8")


def get_active_slot() -> str:
    slot = FLAG_FILE.read_text(encoding="utf-8").strip().upper()
    if slot not in {"A", "B"}:
        raise ValueError("active_slot.txt에는 A 또는 B만 작성할 수 있습니다.")
    return slot


def get_inactive_slot() -> str:
    return "B" if get_active_slot() == "A" else "A"


def install_update(file_data: bytes) -> tuple[str, Path]:
    target_slot = get_inactive_slot()
    target_path = slot_directory(target_slot) / UPDATE_FILENAME
    target_path.write_bytes(file_data)
    return target_slot, target_path


def stop_process(process: subprocess.Popen | None) -> None:
    if process is not None and process.poll() is None:
        process.terminate()
        process.wait(timeout=5)


def launch_slot(slot: str) -> subprocess.Popen | None:
    directory = slot_directory(slot)
    cluster_file = directory / "cluster.py"
    text_file = directory / "slot.txt"

    print(f"[BOOT] Slot {slot}")

    if cluster_file.exists():
        return subprocess.Popen(
            [sys.executable, str(cluster_file)],
            cwd=directory,
        )
    if text_file.exists():
        print(text_file.read_text(encoding="utf-8"))
        return None

    raise FileNotFoundError("활성 Slot에 cluster.py 또는 slot.txt가 없습니다.")


def launch_active_slot() -> subprocess.Popen | None:
    return launch_slot(get_active_slot())


def activate_update(
    target_slot: str,
    running_process: subprocess.Popen | None,
) -> tuple[bool, subprocess.Popen | None]:
    previous_slot = get_active_slot()
    stop_process(running_process)

    try:
        new_process = launch_slot(target_slot)
        if new_process is None:
            raise RuntimeError("업데이트 Slot에 cluster.py가 없습니다.")

        time.sleep(STARTUP_CHECK_SECONDS)
        if new_process.poll() is not None:
            raise RuntimeError(
                f"새 프로그램이 종료되었습니다. exit={new_process.returncode}"
            )

        FLAG_FILE.write_text(target_slot, encoding="utf-8")
        print(f"[COMMIT] Active Slot: {target_slot}")
        return True, new_process

    except Exception as error:
        print(f"[ROLLBACK] Slot {target_slot} 실행 실패: {error}")
        FLAG_FILE.write_text(previous_slot, encoding="utf-8")
        print(f"[ROLLBACK] Active Slot: {previous_slot}")
        return False, launch_slot(previous_slot)


def setup_can_interface() -> None:
    """Linux SocketCAN 인터페이스를 지정한 bitrate로 자동 설정한다."""
    command_prefix = [] if os.geteuid() == 0 else ["sudo"]
    commands = [
        ["ip", "link", "set", CAN_CHANNEL, "down"],
        [
            "ip",
            "link",
            "set",
            CAN_CHANNEL,
            "type",
            "can",
            "bitrate",
            str(CAN_BITRATE),
        ],
        ["ip", "link", "set", CAN_CHANNEL, "up"],
    ]

    try:
        for command in commands:
            subprocess.run(command_prefix + command, check=True)
    except (FileNotFoundError, subprocess.CalledProcessError) as error:
        raise RuntimeError(
            f"{CAN_CHANNEL} 자동 설정에 실패했습니다. "
            "CAN 장치 연결과 sudo 권한을 확인하세요."
        ) from error

    print(f"[CAN] {CAN_CHANNEL} UP, bitrate={CAN_BITRATE}")


def create_can_bus():
    try:
        import can
    except ImportError as error:
        raise RuntimeError(
            "python-can이 필요합니다: pip install python-can"
        ) from error

    setup_can_interface()
    return can.interface.Bus(interface="socketcan", channel=CAN_CHANNEL)


def send_result(bus, result_id: int, transfer_id: int, success: bool) -> None:
    import can

    # Gateway와 동일한 규약: 1은 성공, 0은 실패
    data = struct.pack(">HB", transfer_id, int(success))
    bus.send(can.Message(arbitration_id=result_id, data=data))


def receive_updates(bus) -> None:
    start_id, data_id, end_id, result_id = ECU_CAN_ROUTES[ECU_NAME]
    transfer_id = None
    expected_size = 0
    received_data = bytearray()
    running_process = launch_active_slot()

    print(f"[READY] Update Target: Slot {get_inactive_slot()}")

    try:
        while True:
            message = bus.recv()

            if message.arbitration_id == start_id:
                transfer_id, expected_size = struct.unpack(
                    ">HI", bytes(message.data)
                )
                received_data.clear()
                print(f"[START] ID={transfer_id}, Size={expected_size}")

            elif message.arbitration_id == data_id and transfer_id is not None:
                received_id, sequence = struct.unpack(
                    ">HH", bytes(message.data[:4])
                )
                if received_id == transfer_id:
                    received_data.extend(message.data[4:])
                    print(
                        f"\r[DATA] {sequence}: "
                        f"{len(received_data)}/{expected_size}",
                        end="",
                    )

            elif message.arbitration_id == end_id and transfer_id is not None:
                received_id, _ = struct.unpack(">HH", bytes(message.data))
                if received_id != transfer_id:
                    continue

                print()
                success = len(received_data) == expected_size

                if success:
                    target_slot, target_path = install_update(received_data)
                    print(f"[DONE] {target_path}")
                    success, running_process = activate_update(
                        target_slot,
                        running_process,
                    )

                    if success:
                        print(f"[READY] Update Target: Slot {get_inactive_slot()}")
                else:
                    print(
                        f"[FAIL] expected={expected_size}, "
                        f"actual={len(received_data)}"
                    )

                send_result(bus, result_id, transfer_id, success)
                transfer_id = None
                expected_size = 0
                received_data.clear()
    finally:
        stop_process(running_process)


def main() -> None:
    if ECU_NAME not in ECU_CAN_ROUTES:
        raise ValueError("ECU_NAME은 powertrain, body, adas 중 하나여야 합니다.")

    prepare_slots()
    bus = create_can_bus()
    try:
        receive_updates(bus)
    except KeyboardInterrupt:
        print("\n[STOP] ECU A/B Update 종료")
    finally:
        bus.shutdown()


if __name__ == "__main__":
    main()
