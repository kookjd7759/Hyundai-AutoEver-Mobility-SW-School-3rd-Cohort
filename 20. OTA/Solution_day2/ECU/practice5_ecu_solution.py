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
    print('=== Practice 5 ECU: CAN File Receiver ===')
    output_dir = input('Output Directory [ecu_updates]: ').strip() or 'ecu_updates'
    filename = input('Saved Filename [update.bin]: ').strip() or 'update.bin'
    channel = input('CAN Channel [can0]: ').strip() or 'can0'
    bitrate = int(input('CAN Bitrate [1000000]: ').strip() or '1000000')

    chunks = {}
    can_bus = CANInterface(channel=channel, bitrate=bitrate)
    can_bus.setup()
    print('Waiting for CAN file data...')
    try:
        while True:
            frame = can_bus.receive()
            if frame['id'] == CAN_DATA_ID and len(frame['data']) >= 2:
                sequence = struct.unpack('>H', frame['data'][:2])[0]
                chunks[sequence] = frame['data'][2:]
            elif frame['id'] == CAN_END_ID:
                chunk_count = struct.unpack('>H', frame['data'][:2])[0]
                file_bytes = b''.join(
                    chunks[index] for index in range(chunk_count)
                )
                os.makedirs(output_dir, exist_ok=True)
                output_path = os.path.join(output_dir, os.path.basename(filename))
                with open(output_path, 'wb') as output_file:
                    output_file.write(file_bytes)
                print('CAN file saved:', output_path)
                can_bus.send(CAN_RESULT_ID, b'\x00')
                chunks.clear()
    except (KeyboardInterrupt, KeyError):
        print('Transfer stopped or a CAN chunk was missing.')
    finally:
        can_bus.close()


if __name__ == '__main__':
    main()
