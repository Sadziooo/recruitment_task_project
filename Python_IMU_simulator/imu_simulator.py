import socket
import struct
import signal
import sys

# Rejestry IMU
imu_registers = {
    0x12: [0, 0, 0],  # Akcelerometr (przykładowo)
    0x13: [0, 0, 0],  # Żyroskop (przykładowo)
}

# Wczytanie danych z CSV
def load_csv_data(file_path):
    data = []
    with open(file_path, "r") as file:
        next(file)  # Pomiń nagłówek
        for line in file:
            try:
                data.append([float(val) for val in line.strip().split(",")])
            except ValueError:
                print(f"Warning: skipping line with invalid data: {line.strip()}")
                continue  # Pomiń linię z błędnymi danymi
    return data

csv_data = load_csv_data("imu_data.csv")

# Funkcja obsługująca żądania
def handle_request(data):
    global csv_data
    op_type, reg, length = struct.unpack("!BBH", data[:4])

    if op_type == 0x01:  # Odczyt rejestru
        if reg == 0x12:  # Akcelerometr
            response = struct.pack("!hhh", *csv_data.pop(0))  # Odczytaj linię CSV
        elif reg in imu_registers:
            response = struct.pack("!hhh", *imu_registers[reg])
        else:
            response = b"\x00" * 6  # Pusty rejestr
    elif op_type == 0x02:  # Zapis rejestru
        values = struct.unpack(f"!{length}B", data[4:])
        imu_registers[reg] = list(values)
        response = b"\x00"  # Potwierdzenie zapisu
    else:
        response = b"\xFF"  # Nieznany typ operacji

    return response

# Funkcja do obsługi przerwania
def signal_handler(sig, frame):
    print('Zatrzymywanie serwera...')
    sys.exit(0)

# Serwer UDP
def start_udp_server(host="192.168.0.121", port=63554):  # Zmieniono port na 12346
    server = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
    try:
        server.bind((host, port))
        print(f"UDP Server listening on {host}:{port}")
    except Exception as e:
        print(f"Failed to bind to {host}:{port}, error: {e}")
        return

    # Zarejestruj handler dla sygnału przerwania (Ctrl+C)
    signal.signal(signal.SIGINT, signal_handler)

    while True:
        data, addr = server.recvfrom(1024)
        print(f"Received from {addr}: {data}")

        response = handle_request(data)
        server.sendto(response, addr)

if __name__ == "__main__":
    start_udp_server()
