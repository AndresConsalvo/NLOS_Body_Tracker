from operator import truediv
from tracker import Tracker
from quaternion_calc import updateRotation
import socket
from struct import *

LOCAL_IP = "192.168.1.59"
LOCAL_PORT = 20000

tracker = socket.socket(family=socket.AF_INET, type=socket.SOCK_DGRAM)
tracker.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)

client = socket.socket(socket.AF_INET, socket.SOCK_DGRAM, socket.IPPROTO_UDP)
client.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)


client.bind(("127.0.0.1", 4242))
tracker.bind((LOCAL_IP, LOCAL_PORT))

server_found = False

waist_tracker = Tracker("127.0.0.1", [0.0, 0.0, 0.0], [0.0, 0.0, 0.0], 0.0)

#### body measurements
HMD_to_Head = 0.0
Head_to_Neck = 0.18
Neck_to_Waist = 0.62

#### HMD Pos & Quat
hmd_pos = [0.0, 1.8, 0.0]
hmd_quat = [ 1.0, 0.0, 0.0, 0.0 ]

#### Waist Pos & Quat
waist_pos = [0.0, hmd_pos[1] - Neck_to_Waist, 0.0]
waist_quat = [1.0, 0.0, 0.0, 0.0]

waist_tracker.pos = waist_pos
waist_tracker.quat = waist_quat

#define tracker receive
angle_vector = [0.0, 0.5, 0.0]

while True:
    print("Receiving data!")
    data, addr = client.recvfrom(1024)
    payload_length = len(data)

    if (payload_length == 3):
        header, msg, footer = unpack("=cbc", data)
        print(header, msg, footer)

        if (header == b'P' and footer == b'p'):
            server_found = True
            payload = pack("=cbc", b'P', 45, b'p')
            client.sendto(payload, addr)

    if (server_found):
        print("received message: %s"%data)
        client.sendto(b"hi", addr)
    

    waist_tracker.quat = updateRotation(waist_tracker.quat, angle_vector, verbose=False)