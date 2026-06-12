import os
import struct
import sys


PROJECT_ROOT = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
if PROJECT_ROOT not in sys.path:
    sys.path.insert(0, PROJECT_ROOT)

from CANTransceiver.caninterface import CANInterface


CAN_START_ID = 0x620
CAN_DATA_ID = 0x621
CAN_END_ID = 0x622
CAN_RESULT_ID = 0x623


def main():
    print('=== Practice 6 ECU: CAN Size Verification ===')
    output_dir = input('Output Directory [ecu_updates]: ').strip() or 'ecu_updates'
    filename = input('Saved Filename [update.bin]: ').strip() or 'update.bin'
    channel = input('CAN Channel [can0]: ').strip() or 'can0'
    bitrate = int(input('CAN Bitrate [1000000]: ').strip() or '1000000')

    expected_size = None
    chunks = {}
    can_bus = CANInterface(channel=channel, bitrate=bitrate)
    can_bus.setup()
    print('Waiting for CAN size information and file data...')
    try:
        while True:
            frame = can_bus.receive()
            if frame['id'] == CAN_START_ID:
                expected_size = struct.unpack('>I', frame['data'][:4])[0]
                chunks.clear()
                print('Expected file size:', expected_size)
            elif frame['id'] == CAN_DATA_ID and len(frame['data']) >= 2:
                sequence = struct.unpack('>H', frame['data'][:2])[0]
                chunks[sequence] = frame['data'][2:]
            elif frame['id'] == CAN_END_ID:
                chunk_count = struct.unpack('>H', frame['data'][:2])[0]
                try:
                    file_bytes = b''.join(
                        chunks[index] for index in range(chunk_count)
                    )
                    if expected_size is None or len(file_bytes) != expected_size:
                        raise ValueError(
                            f'size mismatch: expected={expected_size}, '
                            f'actual={len(file_bytes)}'
                        )
                    os.makedirs(output_dir, exist_ok=True)
                    output_path = os.path.join(
                        output_dir,
                        os.path.basename(filename),
                    )
                    with open(output_path, 'wb') as output_file:
                        output_file.write(file_bytes)
                    print('Verified CAN file saved:', output_path)
                    can_bus.send(CAN_RESULT_ID, b'\x00')
                except (KeyError, ValueError) as exc:
                    print('File rejected:', exc)
                    can_bus.send(CAN_RESULT_ID, b'\x01')
                expected_size = None
                chunks.clear()
    except KeyboardInterrupt:
        print('Stopped by user.')
    finally:
        can_bus.close()


if __name__ == '__main__':
    main()
