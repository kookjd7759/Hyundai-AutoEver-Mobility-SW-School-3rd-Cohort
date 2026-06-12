import os
import struct
import sys


PROJECT_ROOT = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
if PROJECT_ROOT not in sys.path:
    sys.path.insert(0, PROJECT_ROOT)

from CANTransceiver.caninterface import CANInterface


CAN_START_ID = 0x630
CAN_DATA_ID = 0x631
CAN_END_ID = 0x632
CAN_RETRY_ID = 0x633
CAN_RESULT_ID = 0x634


def main():
    print('=== Challenge 3 ECU: Missing Chunk Recovery ===')
    output_path = input('Output File Path [recovered.bin]: ').strip() or 'recovered.bin'
    channel = input('CAN Channel [can0]: ').strip() or 'can0'
    bitrate = int(input('CAN Bitrate [1000000]: ').strip() or '1000000')

    transfer_id = None
    expected_size = 0
    chunks = {}
    can_bus = CANInterface(channel=channel, bitrate=bitrate)
    can_bus.setup()
    try:
        while True:
            frame = can_bus.receive()
            if frame['id'] == CAN_START_ID and len(frame['data']) >= 6:
                transfer_id, expected_size = struct.unpack('>HI', frame['data'][:6])
                chunks.clear()
            elif frame['id'] == CAN_DATA_ID and len(frame['data']) >= 4:
                frame_id, sequence = struct.unpack('>HH', frame['data'][:4])
                if frame_id == transfer_id:
                    chunks[sequence] = frame['data'][4:]
            elif frame['id'] == CAN_END_ID and len(frame['data']) >= 4:
                frame_id, chunk_count = struct.unpack('>HH', frame['data'][:4])
                if frame_id != transfer_id:
                    continue
                missing = [
                    sequence
                    for sequence in range(chunk_count)
                    if sequence not in chunks
                ]
                if missing:
                    print('Requesting missing chunk:', missing[0])
                    can_bus.send(
                        CAN_RETRY_ID,
                        struct.pack('>HH', transfer_id, missing[0]),
                    )
                    continue

                file_bytes = b''.join(chunks[index] for index in range(chunk_count))
                if len(file_bytes) != expected_size:
                    print('Recovered file size mismatch.')
                    can_bus.send(
                        CAN_RESULT_ID,
                        struct.pack('>HB', transfer_id, 1),
                    )
                    continue
                output_dir = os.path.dirname(os.path.abspath(output_path))
                os.makedirs(output_dir, exist_ok=True)
                with open(output_path, 'wb') as output_file:
                    output_file.write(file_bytes)
                print('Recovered file saved:', output_path)
                can_bus.send(
                    CAN_RESULT_ID,
                    struct.pack('>HB', transfer_id, 0),
                )
                chunks.clear()
    except KeyboardInterrupt:
        print('Stopped by user.')
    finally:
        can_bus.close()


if __name__ == '__main__':
    main()
