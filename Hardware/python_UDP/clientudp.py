import socket

 

msgFromClient       = "Hello UDP Server"

bytesToSend         = str.encode(msgFromClient)

serverAddressPort   = ("192.168.0.150", 20001)

bufferSize          = 1024

 

# Create a UDP socket at client side

UDPClientSocket = socket.socket(family=socket.AF_INET, type=socket.SOCK_DGRAM)
i = 0
 

# Send to server using created UDP socket
while 1:
    
    UDPClientSocket.sendto(i.to_bytes(2, 'big'), serverAddressPort)
    i = i+1
    if (i > 100):
        i = 0
    

msgFromServer = UDPClientSocket.recvfrom(bufferSize)



msg = "Message from Server {}".format(msgFromServer[0])

print(msg)