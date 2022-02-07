# This script functions as a relay between the ESP32 and Unity, because for some reason the script I wrote for Unity decided it doesn't want to work.

# Import libraries
import os
import socket

# Set send IP adress and port
UDP_IP = "172.20.10.10"
UDP_PORT_Rec = 20001
UDP_PORT_Unity = 20000

print("Receiving on Port:" + str(UDP_PORT_Rec))
print("Sending to IP:" + UDP_IP + ":" + str(UDP_PORT_Unity))

# Set socket to send udp messages and bind port
sock = socket.socket(socket.AF_INET,socket.SOCK_DGRAM)
sock.bind(('',UDP_PORT_Rec))
clearConsole = lambda: os.system('cls' if os.name in ('nt', 'dos') else 'clear')

## Keep sending message from file location
while True:
    print("Beginning receive")
    try:
        data, addr = sock.recvfrom(1024) #buffer size, hopefully no problem if too big
        sock.sendto(data, (UDP_IP, UDP_PORT_Unity))
    except ConnectionResetError:
        print("Connection closed")
    clearConsole()