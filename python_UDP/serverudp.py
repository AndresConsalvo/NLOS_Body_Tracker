import socket
import time
from struct import unpack

 

localIP     = "172.20.10.10"

localPort   = 20000

bufferSize  = 1024

 

msgFromServer       = "Hello UDP Client"

bytesToSend         = str.encode(msgFromServer)

# Create a datagram socket

UDPServerSocket = socket.socket(family=socket.AF_INET, type=socket.SOCK_DGRAM)
 

# Bind to address and ip

UDPServerSocket.bind((localIP, localPort))

 

print("UDP server up and listening!")

count = 0
# Listen for incoming datagrams
while(True):
    bytesAddressPair = UDPServerSocket.recvfrom(bufferSize)

    message = bytesAddressPair[0]

    address = bytesAddressPair[1]

    clientMsg = "Message from Client:{}".format(message)
    clientIP  = "Client IP Address:{}".format(address)

    #tuple_of_shorts = unpack('<' + 'h'*(len(message)//2),message)
    #print(tuple_of_shorts)
    
    print(clientMsg)
    print(clientIP)

    print(type(address))

    #Sending a reply to client

    UDPServerSocket.sendto(message, ('172.20.10.10', 20000))
