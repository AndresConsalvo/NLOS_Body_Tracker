import json
from os import kill
import socket
import timeit
from tracker import Tracker
import pprint
import sys
from psutil import process_iter
from signal import SIGTERM
from ctypes import *


# LOCAL_IP = socket.gethostbyname(socket.gethostname())
LOCAL_IP = "127.0.0.1"
LOCAL_PORT = 20001
BUFFER_SIZE = 1024
ADDR = (LOCAL_IP, LOCAL_PORT)
DISCONNECT = "!DISCONNECT"
recv_counter = 0

UDP_server_socket = socket.socket(family=socket.AF_INET, type=socket.SOCK_DGRAM)

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
  else:
    print("[ERROR] Maximum number of trackers reached")

def update_tracker_info(trackers:dict, tracker:Tracker):
  trackers[tracker.id] = tracker


def start(verbose:bool = False):
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



      if type(payload) is not dict and payload == DISCONNECT:
        listening = False

      elif type(payload) is dict:
        if payload["type"] == DISCONNECT:
          listening = False

        elif payload["type"] == "DEVICE":
          # MESSAGE FROM CLIENT
          print()
          new_tracker = Tracker(payload["data"]["ip"],
                                payload["data"]["accel"],
                                payload["data"]["gyro"],
                                payload["data"]["battery"],
                                payload["data"]["id"],
                                payload["data"]["body_part"])

          new_tracker.battery = payload["data"]["battery"]

          if not (payload["data"]["id"] in trackers):
            store_new_tracker(trackers, new_tracker)
            pprint.pprint(new_tracker.get_device())

          else:
            update_tracker_info(trackers, new_tracker)

          if electron_address:
            # Send data to app
            pprint.pprint(trackers)
            message_to_send = json.dumps(trackers['Tracker-1'].get_device())
            bytes_to_send = str.encode(message_to_send)
            UDP_server_socket.sendto(bytes_to_send, electron_address)


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

      if verbose:
        print("[INFO] message_json:")
        pprint.pprint(payload)
        print(f"[INFO] Client Address:\n{address}\n")


        # if recv_counter == 1:
        #   start = timeit.timeit()

        # if recv_counter%600 == 0:
        #   # stop timer reset counter
        #   end = timeit.timeit()
        #   print(f"[INFO] Time elapsed for 600 messages = {end-start}")
        #   recv_counter = 0

    UDP_server_socket.close()
    print("[DISCONNECTED]  Server Terminated.")


  except KeyboardInterrupt:
    UDP_server_socket.close()
    print("[DISCONNECTED]  Keyboard Interrupt found.")
    pass


if __name__ == "__main__":
  start(eval(sys.argv[1]))
