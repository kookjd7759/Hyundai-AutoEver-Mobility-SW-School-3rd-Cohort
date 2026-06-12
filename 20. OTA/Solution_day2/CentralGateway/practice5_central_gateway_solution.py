import os
import struct
import sys


PROJECT_ROOT = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
if PROJECT_ROOT not in sys.path:
    sys.path.insert(0, PROJECT_ROOT)

from CANTransceiver.caninterface import CANInterface


CAN_DATA_ID = 0x610
CAN_END_ID = 0x611
CAN_RESULT_ID = 0x612


def main():
    print('=== Practice 5 Central Gateway: CAN File Sender ===')
    file_path = input('File Path to Send: ').strip()
    channel = input('CAN Channel [can0]: ').strip() or 'can0'
    bitrate = int(input('CAN Bitrate [1000000]: ').strip() or '1000000')

    if not os.path.isfile(file_path):
        print('File not found:', file_path)
        return
    with open(file_path, 'rb') as input_file:
        file_bytes = input_file.read()

    chunk_count = (len(file_bytes) + 5) // 6
    if chunk_count > 0x10000:
        print('File is too large for the 16-bit sequence number.')
        return

    can_bus = CANInterface(channel=channel, bitrate=bitrate)
    can_bus.setup()
    try:
        for sequence, offset in enumerate(range(0, len(file_bytes), 6)):
            frame = struct.pack('>H', sequence) + file_bytes[offset:offset + 6]
            can_bus.send(CAN_DATA_ID, frame)

        can_bus.send(CAN_END_ID, struct.pack('>H', chunk_count))
        print(f'CAN file transfer complete: {len(file_bytes)} bytes')

        result = can_bus.receive_by_id(CAN_RESULT_ID, timeout=10)
        if result and result['data'][:1] == b'\x00':
            print('ECU confirmed file storage.')
        else:
            print('ECU did not confirm file storage.')
    finally:
        can_bus.close()


if __name__ == '__main__':
    main()
