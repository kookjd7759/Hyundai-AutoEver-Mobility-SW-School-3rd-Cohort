import os
import struct
import sys
import time


PROJECT_ROOT = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
if PROJECT_ROOT not in sys.path:
    sys.path.insert(0, PROJECT_ROOT)

from CANTransceiver.caninterface import CANInterface


CAN_START_ID = 0x630
CAN_DATA_ID = 0x631
CAN_END_ID = 0x632
CAN_RETRY_ID = 0x633
CAN_RESULT_ID = 0x634


def send_chunk(can_bus, transfer_id, sequence, chunks):
    frame = struct.pack('>HH', transfer_id, sequence) + chunks[sequence]
    can_bus.send(CAN_DATA_ID, frame)


def main():
    print('=== Challenge 3 Central Gateway: Missing Chunk Retransmission ===')
    file_path = input('File Path to Send: ').strip()
    omitted_sequence = int(input('Sequence to Omit Once [2]: ').strip() or '2')
    channel = input('CAN Channel [can0]: ').strip() or 'can0'
    bitrate = int(input('CAN Bitrate [1000000]: ').strip() or '1000000')

    if not os.path.isfile(file_path):
        print('File not found:', file_path)
        return
    with open(file_path, 'rb') as input_file:
        file_bytes = input_file.read()

    chunks = [
        file_bytes[offset:offset + 4]
        for offset in range(0, len(file_bytes), 4)
    ]
    if len(chunks) > 0xFFFF:
        print('File is too large.')
        return

    transfer_id = int.from_bytes(os.urandom(2), 'big') or 1
    can_bus = CANInterface(channel=channel, bitrate=bitrate)
    can_bus.setup()
    try:
        can_bus.send(
            CAN_START_ID,
            struct.pack('>HI', transfer_id, len(file_bytes)),
        )
        for sequence in range(len(chunks)):
            if sequence == omitted_sequence:
                print('Intentionally omitted chunk:', sequence)
                continue
            send_chunk(can_bus, transfer_id, sequence, chunks)
        can_bus.send(
            CAN_END_ID,
            struct.pack('>HH', transfer_id, len(chunks)),
        )

        deadline = time.monotonic() + 30
        while time.monotonic() < deadline:
            frame = can_bus.receive(timeout=1)
            if frame is None:
                continue
            if frame['id'] == CAN_RETRY_ID and len(frame['data']) >= 4:
                request_id, sequence = struct.unpack('>HH', frame['data'][:4])
                if request_id == transfer_id and sequence < len(chunks):
                    print('Retransmitting requested chunk:', sequence)
                    send_chunk(can_bus, transfer_id, sequence, chunks)
                    can_bus.send(
                        CAN_END_ID,
                        struct.pack('>HH', transfer_id, len(chunks)),
                    )
            elif frame['id'] == CAN_RESULT_ID and len(frame['data']) >= 3:
                result_id, status = struct.unpack('>HB', frame['data'][:3])
                if result_id == transfer_id:
                    print('Transfer result:', 'success' if status == 0 else 'failed')
                    return
        print('Timed out waiting for ECU result.')
    finally:
        can_bus.close()


if __name__ == '__main__':
    main()
