import os
import can


class CANInterface:
    def __init__(self, channel="can0", bitrate=1000000):
        self.channel = channel
        self.bitrate = bitrate
        self.bus = None

    def setup(self):
        os.system(f"sudo ip link set {self.channel} down")
        os.system(f"sudo ip link set {self.channel} type can bitrate {self.bitrate}")
        os.system(f"sudo ip link set {self.channel} up")

        self.bus = can.interface.Bus(
            channel=self.channel,
            interface="socketcan"
        )

    def close(self):
        if self.bus is not None:
            self.bus.shutdown()
            self.bus = None

        os.system(f"sudo ip link set {self.channel} down")

    def send(self, arb_id, data, extended=False):
        if self.bus is None:
            raise RuntimeError("CAN bus is not initialized. Call setup() first.")
            
        if isinstance(data, str):
            data = data.encode("utf-8")
        else:
            data = bytes(data)

        if len(data) > 8:
            raise ValueError("Classic CAN data length must be 0~8 bytes.")
        
        msg = can.Message(
            arbitration_id=arb_id,
            data=data,
            is_extended_id=extended
        )

        self.bus.send(msg)

        print(
            f"TX -> ID: 0x{arb_id:X} | "
            f"DLC: {len(data)} | "
            f"DATA: {bytes(data).hex(' ')}"
        )

    def receive(self, timeout=None):
        if self.bus is None:
            raise RuntimeError("CAN bus is not initialized. Call setup() first.")

        msg = self.bus.recv(timeout=timeout)

        if msg is None:
            return None

        frame = {
            "id": msg.arbitration_id,
            "dlc": msg.dlc,
            "data": bytes(msg.data),
            "extended": msg.is_extended_id,
            "timestamp": msg.timestamp
        }

        print(
            f"RX <- ID: 0x{frame['id']:X} | "
            f"DLC: {frame['dlc']} | "
            f"DATA: {frame['data'].hex(' ')}"
        )

        return frame

    def receive_by_id(self, arb_id, timeout=None):
        while True:
            frame = self.receive(timeout=timeout)

            if frame is None:
                return None

            if frame["id"] == arb_id:
                return frame
            
if __name__ == "__main__":
    can_device = CANInterface()
    can_device.setup()
    try:
        while True:
            print(can_device.receive())
            
    except KeyboardInterrupt:
        print("Stopped by user.")
    except Exception as e:
        print("Error:", e)
        
    finally:
        can_device.close()
