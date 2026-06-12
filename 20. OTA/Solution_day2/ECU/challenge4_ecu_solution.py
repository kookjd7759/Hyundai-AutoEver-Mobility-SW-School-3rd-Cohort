import os
import struct
import sys


PROJECT_ROOT = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
if PROJECT_ROOT not in sys.path:
    sys.path.insert(0, PROJECT_ROOT)

from CANTransceiver.caninterface import CANInterface


ECU_CAN_ROUTES = {
    'powertrain': {'start': 0x740, 'name': 0x741, 'data': 0x742, 'end': 0x743, 'result': 0x744},
    'body': {'start': 0x750, 'name': 0x751, 'data': 0x752, 'end': 0x753, 'result': 0x754},
    'adas': {'start': 0x760, 'name': 0x761, 'data': 0x762, 'end': 0x763, 'result': 0x764},
}


def main():
    print('=== Challenge 4 ECU: Targeted Package Receiver ===')
    ecu_name = input('ECU Name (powertrain/body/adas): ').strip()
    output_dir = input('Output Directory [ecu_packages]: ').strip() or 'ecu_packages'
    channel = input('CAN Channel [can0]: ').strip() or 'can0'
    bitrate = int(input('CAN Bitrate [1000000]: ').strip() or '1000000')
    if ecu_name not in ECU_CAN_ROUTES:
        print('Unsupported ECU name.')
        return

    route = ECU_CAN_ROUTES[ecu_name]
    transfer_id = None
    expected_size = 0
    filename_size = 0
    name_chunks = {}
    data_chunks = {}

    can_bus = CANInterface(channel=channel, bitrate=bitrate)
    can_bus.setup()
    print(f'{ecu_name} ECU is waiting for its package files.')
    try:
        while True:
            frame = can_bus.receive()
            if frame['id'] == route['start'] and len(frame['data']) >= 7:
                transfer_id, expected_size, filename_size = struct.unpack(
                    '>HIB',
                    frame['data'][:7],
                )
                name_chunks.clear()
                data_chunks.clear()
            elif frame['id'] == route['name'] and len(frame['data']) >= 3:
                frame_id, sequence = struct.unpack('>HB', frame['data'][:3])
                if frame_id == transfer_id:
                    name_chunks[sequence] = frame['data'][3:]
            elif frame['id'] == route['data'] and len(frame['data']) >= 4:
                frame_id, sequence = struct.unpack('>HH', frame['data'][:4])
                if frame_id == transfer_id:
                    data_chunks[sequence] = frame['data'][4:]
            elif frame['id'] == route['end'] and len(frame['data']) >= 4:
                frame_id, chunk_count = struct.unpack('>HH', frame['data'][:4])
                if frame_id != transfer_id:
                    continue
                try:
                    filename_bytes = b''.join(
                        name_chunks[index] for index in sorted(name_chunks)
                    )[:filename_size]
                    filename = os.path.basename(filename_bytes.decode('utf-8'))
                    file_bytes = b''.join(
                        data_chunks[index] for index in range(chunk_count)
                    )
                    if len(file_bytes) != expected_size:
                        raise ValueError('File size mismatch.')
                    os.makedirs(output_dir, exist_ok=True)
                    output_path = os.path.join(output_dir, filename)
                    with open(output_path, 'wb') as output_file:
                        output_file.write(file_bytes)
                    print('Targeted ECU file saved:', output_path)
                    can_bus.send(
                        route['result'],
                        struct.pack('>HB', transfer_id, 0),
                    )
                except Exception as exc:
                    print('Targeted file rejected:', exc)
                    can_bus.send(
                        route['result'],
                        struct.pack('>HB', transfer_id, 1),
                    )
                transfer_id = None
    except KeyboardInterrupt:
        print('Stopped by user.')
    finally:
        can_bus.close()


if __name__ == '__main__':
    main()
