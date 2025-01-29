import socket
import struct

sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)

server_address = ('0.0.0.0', 12345)
sock.bind(server_address)

print("Listening on port 12345...")

while True:
    data, address = sock.recvfrom(1024)
    
    if len(data) >= 7:
        function_code, device_address, register_address, data_length = struct.unpack('<BHHH', data[:7])
        
        frame_data = data[7:]
        print(f"Parsed Frame - Function Code: 0x{function_code:02x}, Device Address: 0x{device_address:02x}, Register Address: 0x{register_address:02x}, Data Length: 0x{data_length}")
        print(f"Frame Data: {frame_data}")
    else:
        print("Received incomplete or malformed data")

    message = "Hello from Simulator!"
    sock.sendto(message.encode('utf-8'), address)
    print(f"Sent message to '{address}': '{message}'")

