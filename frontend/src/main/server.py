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
import keyboard

from psutil import process_iter
from os import kill
import signal
from ctypes import *
from struct import *

# Self-made packages
from tracker import Tracker
from kinematics import *
from math_helpers import *
from util import wlan_ip


LOCAL_HOST = "127.0.0.1"
DRIVER_PORT = 4242
LOCAL_ADDR = (LOCAL_HOST, DRIVER_PORT)

#192.168.1.59
LOCAL_IP = wlan_ip()
LOCAL_PORT = 20000
BUFFER_SIZE = 1024
ADDR = (LOCAL_IP, LOCAL_PORT)
DISCONNECT = "!DISCONNECT"
recv_counter = 0

OPENVR_MESSAGE = "Hello\\x00"
UDP_server_socket = socket.socket(family=socket.AF_INET, type=socket.SOCK_DGRAM)
UDP_server_socket.setsockopt(socket.SOL_SOCKET,socket.SO_RCVBUF, 64)
#UDP_server_socket.settimeout(0.1)

driver = socket.socket(socket.AF_INET, socket.SOCK_DGRAM, socket.IPPROTO_UDP) # UDP
driver.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
driver.setsockopt(socket.SOL_SOCKET,socket.SO_RCVBUF, 64)
#driver.settimeout(0.01)

trackers = {}
trackers[1] = Tracker(0, [0, 0, 0], [0, 0, 0], 0.0, 1)
trackers[2] = Tracker(0, [0, 0, 0], [0, 0, 0], 0.0, 2)
trackers[3] = Tracker(0, [0, 0, 0], [0, 0, 0], 0.0, 3)
trackers[4] = Tracker(0, [0, 0, 0], [0, 0, 0], 0.0, 4)
trackers[5] = Tracker(0, [0, 0, 0], [0, 0, 0], 0.0, 5)
trackers[6] = Tracker(0, [0, 0, 0], [0, 0, 0], 0.0, 6)
tracker_read = [0] * 7

kinematics = Skeleton()

run_server = True
listening = True
driver_found = False
driver_addr = None

start_time = time.time()



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

def start_server_udp(verbose:bool):

  if verbose is None or not isinstance(verbose, bool):
    verbose = False


  print("[STARTING] server is starting...")

  print(f"[BINDING] {ADDR}...")

  unbinded = True
  while unbinded:
    try:
      UDP_server_socket.bind(ADDR)
      unbinded = False
    except OSError:
      for proc in process_iter():
        for conns in proc.connections(kind="inet"):
          if conns.laddr.port == LOCAL_PORT:
            if kill_process_confirm():
              proc.send_signal(signal.SIGTERM)
            else:
              exit(1)


  print("[LISTENING] UDP socket is up and listening")
  

  
  electron_address = None
  openvr_address = None

  
  
  global listening
  print(listening)
  # go = True
  # print_thread = threading.Thread(target=print_trackers, args=(trackers,go,))
  #bytes_address_pair = UDP_server_socket.recvfrom(BUFFER_SIZE)
  while(listening):

    try:
      bytes_address_pair = UDP_server_socket.recvfrom(BUFFER_SIZE)
      message = format(bytes_address_pair[0])
      address = bytes_address_pair[1]
      payload = json.loads(message[2:-1])
    except json.decoder.JSONDecodeError:
      payload = message[2:-1]
      if (not openvr_address):
        openvr_address = address
    except socket.timeout:
      print("Socket timed out.")
      payload = None




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

        tracker = Tracker(address,
                          accel,
                          gyro,
                          4.2,
                          id,
                          None)
        
        if not (id in trackers):
          store_new_tracker(trackers, tracker)
        else:
          update_tracker_info(trackers, tracker)
          trackers.get(id).quat = updateRotation(trackers.get(id).quat, trackers.get(id).gyro)


    if verbose:
      print(f"[INFO] Client Address:\n{address}\n")
      print("[INFO] message_json:")
      pprint.pprint(payload)

  UDP_server_socket.close()
  print("[DISCONNECTED]  Server Terminated.")

  return 0

def start_driver_udp():
  driver.bind(LOCAL_ADDR)

  global listening
  global driver_addr
  global driver_found



  while(listening):

    # Request data once trackers have all received data

    try:
      data, driver_addr = driver.recvfrom(BUFFER_SIZE)
      #print("--- Data received: %s seconds ---" % (time.time() - start_time))
      payload_length = len(data)

      if (payload_length == 3):
        header, msg, footer = unpack("=cbc", data)

        if (header == b'P' and footer == b'p'):
            #print("Driver found!")
            driver_found = True
            payload = pack("=cbc", b'P', 45, b'p')
            driver.sendto(payload, driver_addr)

      if (payload_length == 31):
        #print("Sending data!")
        #print("--- HMD received: %s seconds ---" % (time.time() - start_time))
        header, x, y, z, qw, qx, qy, qz, id, footer = unpack("=cfffffffbc", data)
        if (header == b'H' and footer == b'h'):
          hmd_pos = quaternion(0, x, y, z)
          hmd_quat = quaternion(qw, qx, qy, qz)

          update_body(kinematics, trackers, hmd_pos, hmd_quat, verbose=False)
          #print("--- Body updated: %s seconds ---" % (time.time() - start_time))
          
          for i in trackers:
            #print("--- Getting tracker: %s seconds ---" % (time.time() - start_time))
            track = trackers[i]
            id = track.id
            quat = track.quat
            pos = track.pos
            #print("--- Payload packing: %s seconds ---" % (time.time() - start_time))
            payload = pack('=cfffffffbc', b'T', pos.x, pos.y, pos.z, quat.w, quat.x, quat.y, quat.z, id, b't')
            #print("--- Payload packed: %s seconds ---" % (time.time() - start_time))
            driver.sendto(payload, driver_addr)
            #print("--- Payload sent: %s seconds ---" % (time.time() - start_time))
        #print("--- Send done: %s seconds ---" % (time.time() - start_time))

    except socket.timeout:
      print("socket timed out")
      #driver_addr = None
    except ConnectionResetError:
      driver_addr = None
      print("Connection lost!")

    
          # if(sum(tracker_read) == 3):
          # 

  driver.close()
  print("Ending driver udp socket")

def request_hmd():
  global driver_found
  global driver_addr
  while True:
    if (driver_addr is not None and driver_found is True):
      payload = pack('=cbc', b'C', 0, b'c')
      driver.sendto(payload, driver_addr)
    time.sleep(0.00001)


def sigint_handler(signum, frame):
  global listening
  global run_server

  print("Ctrl+C was entered!")
  listening = False
  run_server = False
  exit(0)

def calibrate(trackers):
  for i in trackers:
    trackers[i].quat = quaternion(1, 0, 0, 0)
  return

if __name__ == "__main__":
  parser = argparse.ArgumentParser()
  parser.add_argument('-v', type=bool)

  # start two threads: One that handles data from the tracker, one that handles sending data to the driver. 

  print(LOCAL_IP)
  signal.signal(signal.SIGINT, sigint_handler)
  args = parser.parse_args()
  server_udp = threading.Thread(target=start_server_udp, args=(False,), daemon=True)
  server_udp.start()

  driver_udp = threading.Thread(target=start_driver_udp, daemon=True)
  driver_udp.start()

  #request_udp = threading.Thread(target=request_hmd, daemon=True)
  #request_udp.start()

  counter = 0
  while(run_server):
    if keyboard.is_pressed('x'):  # if key 'x' is pressed 
        print('Calibrating!')
        calibrate(trackers)
        #break  # finishing the loop
    counter = counter + 1

  server_udp.join()
  driver_udp.join()
  #request_udp.join()
  #start(args.v)
