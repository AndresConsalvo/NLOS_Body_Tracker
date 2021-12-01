import socket

 

msgFromClient       = "Hello UDP Server"

bytesToSend         = str.encode(msgFromClient)

serverAddressPort   = ("172.20.10.10", 20000)

bufferSize          = 1024

 

# Create a UDP socket at client side

UDPClientSocket = socket.socket(family=socket.AF_INET, type=socket.SOCK_DGRAM)

 

# Send to server using created UDP socket
while 1:
    UDPClientSocket.sendto(bytesToSend, serverAddressPort)

    

msgFromServer = UDPClientSocket.recvfrom(bufferSize)



msg = "Message from Server {}".format(msgFromServer[0])

print(msg)