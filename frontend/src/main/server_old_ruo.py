import json
from lib2to3.pgen2.driver import Driver
import socket
import time
import timeit
import pprint
import sys
import argparse
import threading
import struct
from tracker import Tracker
from psutil import process_iter
from os import kill
from signal import SIGTERM
from ctypes import *
from kinematics import *
from math_helpers import *
from struct import *



# LOCAL_IP = socket.gethostbyname(socket.gethostname())
LOCAL_HOST = "127.0.0.1"
DRIVER_PORT = 4242
LOCAL_ADDR = (LOCAL_HOST, DRIVER_PORT)

LOCAL_IP = "192.168.1.59"
LOCAL_PORT = 20000
BUFFER_SIZE = 1024
ADDR = (LOCAL_IP, LOCAL_PORT)
DISCONNECT = "!DISCONNECT"
recv_counter = 0

OPENVR_MESSAGE = "Hello\\x00"
UDP_server_socket = socket.socket(family=socket.AF_INET, type=socket.SOCK_DGRAM)

client = socket.socket(socket.AF_INET, socket.SOCK_DGRAM, socket.IPPROTO_UDP) # UDP
client.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)




def print_trackers(trackers:dict, go):
  while go:
    for tracker in trackers.values():
      print(f"[INFO] Trackers data:")
      pprint.pprint(tracker.get_device())
    time.sleep(2)

def kill_process_confirm() -> bool:
  msg = f"Another process has been found using port {LOCAL_PORT}.\n"\
         "Do you want to close this process to continue?"
  return True if open_popup(text=msg) == 6 else False


def open_popup(title:str="WARNING!", text:str="Text") -> int:
  _MessageBox = WinDLL("user32").MessageBoxW
  return _MessageBox(0, text, title, 4)


def store_new_tracker(trackers:dict, tracker:Tracker):
  if len(trackers) < 7:
    trackers[tracker.id] = tracker
    return True
  else:
    print("[ERROR] Maximum number of trackers reached")
    return False

def update_tracker_info(trackers:dict, tracker:Tracker, body_part="", body_part_update=False):
  # print("[TEST]--->update_trackers_info")
  # pprint.pprint(trackers.get(tracker.id).get_device())

  trackers.get(tracker.id).accel = tracker.accel
  trackers.get(tracker.id).gyro  = tracker.gyro
  if body_part_update:
    trackers.get(tracker.id).body_part = body_part



def start(verbose:bool):
  if verbose is None or not isinstance(verbose, bool):
    verbose = False


  print("[STARTING] server is starting...")
  recv_counter = 0

  print(f"[BINDING] {ADDR}...")

  unbinded = True
  while unbinded:
    try:
      client.bind(LOCAL_ADDR)
      UDP_server_socket.bind(ADDR)
      unbinded = False
    except OSError:
      for proc in process_iter():
        for conns in proc.connections(kind="inet"):
          if conns.laddr.port == LOCAL_PORT:
            if kill_process_confirm():
              proc.send_signal(SIGTERM)
            else:
              exit(1)


  print("[LISTENING] UDP socket is up and listening")
  listening = True

  trackers = {}
  electron_address = None
  openvr_address = None

  tracker_read = [0] * 7
  
  driver_addr = []
  driver_connected = False
  kinematics = Skeleton()

  # go = True
  # print_thread = threading.Thread(target=print_trackers, args=(trackers,go,))
  #bytes_address_pair = UDP_server_socket.recvfrom(BUFFER_SIZE)
  try:
    while(listening):
      while(driver_connected == False):
        try:
          data, bytes_address_pair = client.recvfrom(BUFFER_SIZE)
          payload_length = len(data)

          if (payload_length == 3):
            header, msg, footer = unpack("=cbc", data)
            print(header, msg, footer)

            if (header == b'P' and footer == b'p'):
                print("Driver found!")
                driver_connected = True
                payload = pack("=cbc", b'P', 45, b'p')
                client.sendto(payload, bytes_address_pair)
                driver_addr = bytes_address_pair

        except ConnectionResetError:
          openvr_address = []
          print("Connection lost!")

      bytes_address_pair = UDP_server_socket.recvfrom(BUFFER_SIZE)
      message = format(bytes_address_pair[0])
      address = bytes_address_pair[1]
      try:
        payload = json.loads(message[2:-1])
      except json.decoder.JSONDecodeError:
        payload = message[2:-1]
        if (not openvr_address):
          openvr_address = address
          print(openvr_address)



      # print(f"[PAYLOAD]--->{payload}")
      # print(OPENVR_MESSAGE == payload)


      if type(payload) is not dict and payload == DISCONNECT:
        listening = False

      elif type(payload) is dict:
        if payload["type"] == DISCONNECT:
          listening = False

        elif payload["type"] == "DEVICE":
          # MESSAGE FROM CLIENT
          new_tracker = Tracker(payload["data"]["ip"],
                                payload["data"]["accel"],
                                payload["data"]["gyro"],
                                payload["data"]["battery"],
                                payload["data"]["id"],
                                payload["data"]["body_part"])

          new_tracker.battery = payload["data"]["battery"]

          if not (payload["data"]["id"] in trackers):
            store_new_tracker(trackers, new_tracker)
            # pprint.pprint(new_tracker.get_device())

          else:
            update_tracker_info(trackers, new_tracker)

          if electron_address:
            # Send data to app
            # pprint.pprint(trackers)
            message_to_send = json.dumps(trackers[payload["data"]["id"]].get_device())
            bytes_to_send = str.encode(message_to_send)
            UDP_server_socket.sendto(bytes_to_send, openvr_address)


        elif payload["type"] == "DEVICE_STATS":
          pass

        elif payload["type"] == "ELECTRON_HAND_SHAKE":
          # message_from_server = "[CONNECTED] App and Server are communicating."
          # bytes_to_send = str.encode(message_from_server)
          # print(bytes_to_send)
          # UDP_server_socket.sendto(bytes_to_send, address)
          electron_address = address
          pass

        elif payload["type"] == "CHANGE_ROLE":
          pass

        elif payload["type"] == "POSITION":
          accel = payload["data"]["accel"]
          gyro  = payload["data"]["gyro"]
          id    = payload["data"]["id"]

          print(gyro)

          tracker = Tracker(address,
                            accel,
                            gyro,
                            4.2,
                            id,
                            None)
          
          print(gyro)
          if not (id in trackers):
            store_new_tracker(trackers, tracker)
          else:
            update_tracker_info(trackers, tracker)
            trackers.get(id).quat = updateRotation(trackers.get(id).quat, trackers.get(id).gyro)
          
          tracker_read[id] = 1
          print(id)
          print(sum(tracker_read))

          if(sum(tracker_read) == 3):
            data, bytes_address_pair = client.recvfrom(BUFFER_SIZE)
            if (len(data) == 31):
              header, x, y, z, qw, qx, qy, qz, id, footer = unpack("=cfffffffbc", data)
              hmd_pos = quaternion(0, x, y, z)
              hmd_quat = quaternion(qw, qx, qy, qz)
              update_body(kinematics, trackers, hmd_pos, hmd_quat, verbose=True)

              for i in trackers:
                track = trackers.get(i)
                id = track.id
                quat = track.quat
                pos = track.pos
                payload = pack('=cfffffffbc', b'T', pos.x, pos.y, pos.z, quat.w, quat.x, quat.y, quat.z, id, b't')
                client.sendto(payload, driver_addr)

              

          if openvr_address:

              arr = payload["data"]["gyro"]
              arr.append(float(tracker.id))
              message_to_send = arr
              bytes_to_send = struct.pack('%sf' % len(message_to_send), *message_to_send)
              print(f'[SIZE OF PCKG]--->{sys.getsizeof(bytes_to_send)}')
              UDP_server_socket.sendto(bytes_to_send, openvr_address)


      if verbose:
        print(f"[INFO] Client Address:\n{address}\n")
        print("[INFO] message_json:")
        pprint.pprint(payload)
        # if not print_thread.is_alive():
        #   print('is not alive')
        #   print_thread.start()



      # for t in trackers.values():
      #   pprint.pprint(t.get_device())


        # if recv_counter == 1:
        #   start = timeit.timeit()

        # if recv_counter%600 == 0:
        #   # stop timer reset counter
        #   end = timeit.timeit()
        #   print(f"[INFO] Time elapsed for 600 messages = {end-start}")
        #   recv_counter = 0
    # go = False
    # print_thread.join()
    UDP_server_socket.close()
    print("[DISCONNECTED]  Server Terminated.")


  except KeyboardInterrupt:
    UDP_server_socket.close()
    print("[DISCONNECTED]  Keyboard Interrupt found.")
    pass


if __name__ == "__main__":
  parser = argparse.ArgumentParser()
  parser.add_argument('-v', type=bool)

  # start two threads: One that handles data from the tracker, one that handles sending data to the driver.
  


  args = parser.parse_args()
  start(args.v)
