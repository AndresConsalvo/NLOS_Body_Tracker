import json
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



# LOCAL_IP = socket.gethostbyname(socket.gethostname())
LOCAL_IP = "192.168.1.51"
LOCAL_PORT = 20000
BUFFER_SIZE = 1024
ADDR = (LOCAL_IP, LOCAL_PORT)
DISCONNECT = "!DISCONNECT"
recv_counter = 0

OPENVR_MESSAGE = "Hello\\x00"
UDP_server_socket = socket.socket(family=socket.AF_INET, type=socket.SOCK_DGRAM)

<<<<<<< HEAD
def update_app(t, sock:socket.socket, trackers:dict[str, Tracker], address, c):
  if address:
    # while True:
    for track in trackers.values():
      message_to_send = json.dumps(track.get_device())
      bytes_to_send = str.encode(message_to_send)
      sock.sendto(bytes_to_send, address)
  time.sleep(t)

def print_trackers(trackers:dict[str,Tracker], go):
=======
def print_trackers(trackers:dict, go):
>>>>>>> 20bb9ed554344aad14feae1881078eae6e9fcdf0
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

  # go = True
  # print_thread = threading.Thread(target=print_trackers, args=(trackers,go,))

  count = 0
  t1 = threading.Thread(target=update_app, args=(10,UDP_server_socket,trackers,addresses["electron"],count))
  t1.start()
  try:
    while(listening):

      bytes_address_pair = UDP_server_socket.recvfrom(BUFFER_SIZE)

      recv_counter += 1

      message = format(bytes_address_pair[0])
      address = bytes_address_pair[1]
      try:
        payload = json.loads(message[2:-1])
      except json.decoder.JSONDecodeError:
        payload = message[2:-1]
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

          if openvr_address:

              arr = payload["data"]["gyro"]
              arr.append(float(tracker.id))
              message_to_send = arr
              bytes_to_send = struct.pack('%sf' % len(message_to_send), *message_to_send)
              print(f'[SIZE OF PCKG]--->{sys.getsizeof(bytes_to_send)}')
              UDP_server_socket.sendto(bytes_to_send, openvr_address)


      if not t1.is_alive():
        print('[START THREAD]')
        t1 = threading.Thread(target=update_app, args=(1,UDP_server_socket,trackers,addresses["electron"],count))
        t1.start()


      # t1.join()
      count += 1
      if verbose:
        print(f"[INFO] Client Address:\n{address}\n")
        print("[INFO] message_json:")
        pprint.pprint(payload)
    UDP_server_socket.close()
    print("[DISCONNECTED]  Server Terminated.")


  except KeyboardInterrupt:
    UDP_server_socket.close()
    print("[DISCONNECTED]  Keyboard Interrupt found.")
    pass


if __name__ == "__main__":
  parser = argparse.ArgumentParser()
  parser.add_argument('-v', type=bool)


  args = parser.parse_args()
  start(args.v)
