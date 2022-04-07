import serial
import socket

ssid = input("Enter SSID: ")
password = input("Enter password: ")
ip = input("Enter IP: ")
ip_bytes = socket.inet_aton(ip)
print(ip_bytes)
print(len(ip_bytes))

port = input("Enter port: ")
port = int(port)

print(port)
port = port.to_bytes(2, 'big')
print(port.hex())

send_str = ssid.encode() + b'\x0A' + password.encode() + b'x0A' + ip_bytes + b'x0A' + port + b'\x0A'

print(send_str)
