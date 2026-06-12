import os
import struct
import sys


PROJECT_ROOT = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
if PROJECT_ROOT not in sys.path:
    sys.path.insert(0, PROJECT_ROOT)

from CANTransceiver.caninterface import CANInterface


ECU_CAN_ROUTES = {
    'powertrain': {'start': 0x700, 'data': 0x701, 'end': 0x702, 'result': 0x703},
    'body': {'start': 0x710, 'data': 0x711, 'end': 0x712, 'result': 0x713},
    'adas': {'start': 0x720, 'data': 0x721, 'end': 0x722, 'result': 0x723},
}


def main():
    print('=== Practice 7 ECU: End-to-End OTA Receiver ===')
    ecu_name = input('ECU Name (powertrain/body/adas) [powertrain]: ').strip() or 'powertrain'
    output_dir = input('Output Directory [ecu_updates]: ').strip() or 'ecu_updates'
    filename = input('Saved Filename [update.bin]: ').strip() or 'update.bin'
    channel = input('CAN Channel [can0]: ').strip() or 'can0'
    bitrate = int(input('CAN Bitrate [1000000]: ').strip() or '1000000')
    force_fail_once = (
        input('Force first verification failure? (y/n) [n]: ').strip().lower()
        == 'y'
    )

    if ecu_name not in ECU_CAN_ROUTES:
        print('Unsupported ECU. Choose powertrain, body, or adas.')
        return
    route = ECU_CAN_ROUTES[ecu_name]

    transfer_id = None
    expected_size = None
    chunks = {}
    can_bus = CANInterface(channel=channel, bitrate=bitrate)
    can_bus.setup()
    print(f'{ecu_name} ECU is waiting for routed CAN frames.')
    try:
        while True:
            frame = can_bus.receive()
            if frame['id'] == route['start'] and len(frame['data']) >= 6:
                transfer_id, expected_size = struct.unpack(
                    '>HI',
                    frame['data'][:6],
                )
                chunks.clear()
                print(
                    f'Started transfer: id={transfer_id}, '
                    f'expected size={expected_size}'
                )
            elif frame['id'] == route['data'] and len(frame['data']) >= 4:
                frame_transfer_id, sequence = struct.unpack(
                    '>HH',
                    frame['data'][:4],
                )
                if frame_transfer_id == transfer_id:
                    chunks[sequence] = frame['data'][4:]
            elif frame['id'] == route['end'] and len(frame['data']) >= 4:
                end_transfer_id, chunk_count = struct.unpack(
                    '>HH',
                    frame['data'][:4],
                )
                if end_transfer_id != transfer_id:
                    continue

                try:
                    file_bytes = b''.join(
                        chunks[index] for index in range(chunk_count)
                    )
                    if force_fail_once:
                        force_fail_once = False
                        raise ValueError('forced ECU failure')
                    if len(file_bytes) != expected_size:
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
                    print('ECU verified and saved update:', output_path)
                    can_bus.send(
                        route['result'],
                        struct.pack('>HB', transfer_id, 0),
                    )
                except (KeyError, ValueError) as exc:
                    print('ECU rejected update:', exc)
                    can_bus.send(
                        route['result'],
                        struct.pack('>HB', transfer_id, 1),
                    )
                transfer_id = None
                expected_size = None
                chunks.clear()
    except KeyboardInterrupt:
        print('Stopped by user.')
    finally:
        can_bus.close()


if __name__ == '__main__':
    main()
