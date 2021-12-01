# Import libraries
import socket
import time

# Set send IP adress and port
UDP_IP = "172.20.10.10"
UDP_PORT_Rec = 20001
UDP_PORT_Unity = 20000

print("Receiving on Port:" + str(UDP_PORT_Rec))
print("Sending to IP:" + UDP_IP + ":" + str(UDP_PORT_Unity))

# Set socket to send udp messages and bind port
sock = socket.socket(socket.AF_INET,socket.SOCK_DGRAM)
sock.bind(('',UDP_PORT_Rec))

## Keep sending message from file location
while True:
    print("Beginning receive")
    data, addr = sock.recvfrom(1024) #buffer size, hopefully no problem if too big
    print("Received")

    sock.sendto(data, (UDP_IP, UDP_PORT_Unity))
    print("Send")