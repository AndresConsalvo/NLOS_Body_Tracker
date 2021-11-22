import socket
import time
import timer
from struct import unpack

 

localIP     = "192.168.1.31"

localPort   = 20001

bufferSize  = 1024

 

msgFromServer       = "Hello UDP Client"

bytesToSend         = str.encode(msgFromServer)

 

# Create a datagram socket

UDPServerSocket = socket.socket(family=socket.AF_INET, type=socket.SOCK_DGRAM)

 

# Bind to address and ip

UDPServerSocket.bind((localIP, localPort))

 

print("UDP server up and listening")

count = 0
start_time = time.process_time()
total_time = 0
# Listen for incoming datagrams
while(True):
    bytesAddressPair = UDPServerSocket.recvfrom(bufferSize)


    count = count + 1
    total_time = total_time + (time.process_time() - start_time)
    print(total_time)
    if (total_time > 1.00):
        total_time = 0
        print(count)
        count = 0


   
    #message = bytesAddressPair[0]

    #address = bytesAddressPair[1]

    #clientMsg = "Message from Client:{}".format(message)
    #clientIP  = "Client IP Address:{}".format(address)

    #tuple_of_shorts = unpack('<' + 'h'*(len(message)//2),message)
    #print(tuple_of_shorts)
    
    #print(clientMsg)
    #print(clientIP)

   

    # Sending a reply to client

    #UDPServerSocket.sendto(bytesToSend, address)



 