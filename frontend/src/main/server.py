import json
import socket
from io import StringIO
 

localIP     = "127.0.0.1"

localPort   = 20001
localPortElectron   = 20005

bufferSize  = 1024

 

msgFromServer       = "Hello UDP Client"

bytesToSend         = str.encode(msgFromServer)

 

# Create a datagram socket
print('antes del socke.socket')
UDPServerSocket = socket.socket(family=socket.AF_INET, type=socket.SOCK_DGRAM)
UDPServerSocketElectron = socket.socket(family=socket.AF_INET, type=socket.SOCK_DGRAM)
 

# Bind to address and ip
print('antes del bind')
UDPServerSocket.bind((localIP, localPort))
# UDPServerSocketElectron.bind((localIP, localPortElectron))

 

print("UDP server up and listening")

sensors = {}

# Listen for incoming datagrams

electronAddress = None

while(True):
    bytesAddressPair = UDPServerSocket.recvfrom(bufferSize)
    

    message = format(bytesAddressPair[0] )   
    address = bytesAddressPair[1]
    # esto explota
    # print(message)
    # print(json.loads(message[2:-1]))
    receivedMessage = message
    try:
        receivedMessage = json.loads(message[2:-1])
        print(receivedMessage)
    finally: 
    
        print("Message from Client:", receivedMessage)
        print("Client IP Address:", address)
        if receivedMessage['type'] == 'ELECTRON_HAND_SHAKE':
            electronAddress = address

        elif  receivedMessage['type'] == 'DEVICE_STATS' and electronAddress :
            sensors[receivedMessage['data']['ip']] = receivedMessage['data']
            print('sensors', sensors)
            UDPServerSocket.sendto(bytesAddressPair[0], electronAddress)
        elif  receivedMessage['type'] == 'CHANGE_ROLE' : 
            # do stuff
            print(receivedMessage['data']['ip'])
            print(sensors[receivedMessage['data']['ip']])
            sensors[receivedMessage['data']['ip']]['role'] = receivedMessage['data']['role']
            print('sensors', sensors)
            UDPServerSocket.sendto(str.encode(json.dumps({
                'type': 'DEVICE_STATS',
                'data': sensors[receivedMessage['data']['ip']]
            })), electronAddress)

    
    # response =  json.loads(receivedMessage)   

    # Sending a reply to client
    
    # UDPServerSocketElectron.sendto(response,(localIP, localPortElectron))
    # if(address == electronAddress):
    #     UDPServerSocket.sendto(str.encode('RESPONSE'), address)
    # else : 
    #     UDPServerSocket.sendto(str.encode('RESPONSE'), address)
    #     UDPServerSocket.sendto(str.encode('RESPONSE'), electronAddress)

