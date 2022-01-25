import socket
import time
import random
import json

   
msgFromClient       = str(random.randint(3, 9))
serverAddressPort   = ("127.0.0.1", 20001)
bufferSize          = 1024

# Create a UDP socket at client side
UDPClientSocket = socket.socket(family=socket.AF_INET, type=socket.SOCK_DGRAM)

# Send to server using created UDP socket

for i in range(30):
  values = [random.randint(3, 9), random.randint(3, 9), random.randint(3, 9), random.randint(3, 9), random.randint(3, 9),random.randint(3, 9) ]
  response = {
    "type": 'DEVICE_STATS',
    "data": {
      "ip": "192.168.0.136", 
      "position":values,
      "battery": '15%',
      "battery_V": 4.18,
      "MAC_ADRESS": '2345678_name1',
      "IMU_accuracy": "High",
      # "role": 'Right Ankle'
    }
  }
  msgFromClient= json.dumps(response)
  bytesToSend = str.encode(msgFromClient)
  UDPClientSocket.sendto(bytesToSend, serverAddressPort)

  # msgFromServer = UDPClientSocket.recvfrom(bufferSize)
  # msg = "Message from Server {}".format(msgFromServer[0])
  # print(msg)

  time.sleep(1)