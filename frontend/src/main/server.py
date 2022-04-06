import json
import socket
import time
import timeit
import pprint
import sys
import argparse
import threading
import struct
import os
import signal
from dotenv import load_dotenv
from psutil import process_iter
from os import kill
from signal import SIGTERM
from ctypes import *

from ctypes import *
from struct import *

from tracker import Tracker
from server_events import ServerEvent

# Rou's Packages
from tracker import Tracker
from kinematics import *
from math_helpers import *
from util import wlan_ip
load_dotenv()

# COMMUNICATE WITH THE DRIVER/OPENVR
LOCAL_HOST = os.getenv("LOCAL_HOST")
DRIVER_PORT = 4242
DRIVER_ADDR = (LOCAL_HOST, DRIVER_PORT)

# COMMUNICATE WITH THE TRACKERS OVER WIFI
LOCAL_IP = wlan_ip
BUFFER_SIZE = int(os.getenv("BUFFER_SIZE"))
TRACKERS_PORT = 20000
TRACKERS_ADDR = (LOCAL_IP,   TRACKERS_PORT)

# COMMUNICATE WITH THE GUI OVER LOCALHOST
GUI_PORT = int(os.getenv("LOCAL_PORT"))
GUI_ADDR = (LOCAL_HOST, GUI_PORT)


DISCONNECT = "!DISCONNECT"
OPENVR_MESSAGE = "Hello\\x00"

# OpenVR driver/server communication UDP socket
driver_server_socket = socket.socket(socket.AF_INET, socket.SOCK_DGRAM, socket.IPPROTO_UDP)
driver_server_socket.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
driver_server_socket.setsockopt(socket.SOL_SOCKET, socket.SO_RCVBUF, 64)

# Trackers/Server communication UDP socket
trackers_server_socket = socket.socket(family=socket.AF_INET, type=socket.SOCK_DGRAM)
trackers_server_socket.setsockopt(socket.SOL_SOCKET, socket.SO_RCVBUF, 64)

# GUI/Server communication UDP socket
gui_server_socket = socket.socket(family=socket.AF_INET, type=socket.SOCK_DGRAM)



recv_counter = 0

addresses = { "openvr": None,
              "electron": None}

trackers:dict[int, Tracker] = {}
for i in range (6):
  trackers[i+1] = Tracker(socket.gethostbyname(socket.gethostname()), [0, 0, 0], [0, 0, 0], 4.0, i, i+1)

kinematics = Skeleton()

body_measurements = {}
server_events = ServerEvent()
server_events.addresses = addresses
server_events.trackers = trackers
server_events.body_measurements = body_measurements

run_server = True
listening = True
driver_found = False
driver_addr = None
calibrate = False

def update_app(t, sock:socket.socket, trackers:dict[str, Tracker], address):
  # ************
  # Sends the trackers information to the GUI. Then pauses
  # the thread for the specified period of time.
  #
  # @param {int} [t] The period of time to pause the thread.
  # @param {socket.socket} [sock] The socket where the message
  #        will be sent through.
  # @param {dict[str, Tracker]} [trackers] A dictionary containing
  #        the trackers with the info to be sent to the GUI.
  # @param {_Address} [address] The address of the GUI.
  # ************

  if address:
    for track in trackers.values():
      message_to_send = json.dumps(track.get_device())
      bytes_to_send = str.encode(message_to_send)
      sock.sendto(bytes_to_send, address)
  time.sleep(t)

def kill_process_confirm(port) -> bool:
  # ************
  # Calls the open_popup function and prompts the user to kill
  # or continue a process.
  #
  # @return {bool} If the user chooses to close the process
  #         returns true, if not then returns false
  # ************

  msg = f"Another process has been found using port {port}.\n"\
         "Do you want to close this process to continue?"
  return True if open_popup(text=msg) == 6 else False

def open_popup(title:str="WARNING!", text:str="Text") -> int:
  # ************
  # Open a Windows pop-up window with the choices `Yes` and `No`.
  #
  # @param {str} [title="WARNING!"] The title of the pop-up window.
  # @param {str} [text="Text"] The question to be answered by the user
  #        in terms of `yes` or `no`.
  # @return {MessageBoxW} Returns the answer chosen by the user.
  # ************

  _MessageBox = WinDLL("user32").MessageBoxW
  return _MessageBox(0, text, title, 4)

def bind(sock:socket.socket, port:int, addr:tuple):
  # ************
  # Binds an UDP socket with a port, checking if the specific port
  # is in use, a pop-up window will appear and ask the user permision
  # to close the process using the port and address. If permision is
  # denied then the program will exit.
  #
  # @param {socket.socket} [sock] socket to be binded with the port a
  #                                and IP address.
  # @param {int} [port] the network port to be check for binding.
  # @param {tuple(string,int)} [addr] tuple with the IP address and port.
  # ************

  binded = False
  while not binded:
    try:
      sock.bind(addr)
      binded = True
    except OSError:
      for proc in process_iter():
        for conns in proc.connections(kind="inet"):
          if conns.laddr.port == port:
            if kill_process_confirm(port):
              proc.send_signal(SIGTERM)
            else:
              exit(1)

def communicate_gui_udp(verbose:bool):
  # ************
  # Binds the GUI-Server socket with the designated port.
  # Begins communications with the GUI.
  #
  # @params {bool} [verbose] Selects whether to output the
  #         transmited data to the console or not.
  # ************

  global GUI_PORT
  global recv_counter
  global listening

  if verbose is None or not isinstance(verbose, bool):
    verbose = False

  print("[STARTING] GUI server is starting...")
  print(f"[BINDING] {GUI_ADDR}...")
  bind(gui_server_socket, GUI_PORT, GUI_ADDR)

  print("[LISTENING] UDP socket is up and listening")



  while(listening):
    try:

      bytes_address_pair = gui_server_socket.recvfrom(BUFFER_SIZE)

      recv_counter += 1

      message = format(bytes_address_pair[0])
      address = bytes_address_pair[1]

      try:
        payload = json.loads(message[2:-1])
      except json.decoder.JSONDecodeError:
        payload = message[2:-1]
        addresses["openvr"] = address

      if type(payload) is dict:
        if payload["type"] == DISCONNECT:
          listening = False

        else:
          server_events.handle_event(payload, gui_server_socket, address)

      if verbose:
        print(f"[INFO] Client Address:\n{address}\n")
        print("[INFO] message_json:")
        pprint.pprint(payload)

    except socket.timeout:
      print("[DISCONNECTED] GUI/Server socket timeout")

    except ConnectionResetError:
      print("[DISCONNECTED] GUI/Server connection lost")

  gui_server_socket.close()
  print("[DISCONNECTED] GUI/Server socket terminated")

def communicate_trackers_udp(verbose:bool):
  # ************
  # Binds the Trackers-Server socket with the designated port.
  # Begins communications with the Trackers.
  # ************

  global TRACKERS_PORT
  global recv_counter
  global listening

  if verbose is None or not isinstance(verbose, bool):
    verbose = False

  print("[STARTING] GUI server is starting...")
  print(f"[BINDING] {TRACKERS_ADDR}...")
  bind(gui_server_socket, TRACKERS_PORT, TRACKERS_ADDR)

  print("[LISTENING] UDP socket is up and listening")

  while (listening):
    try:
      bytes_address_pair = trackers_server_socket.recvfrom(BUFFER_SIZE)
      message = format(bytes_address_pair[0])
      address = bytes_address_pair[1]

      try:
        payload = json.loads(message[2:-1])
      except json.decoder.JSONDecodeError:
        payload = message[2:-1]
        addresses["openvr"] = address

      if type(payload) is dict:
        if payload["type"] == DISCONNECT:
          listening = False

        else:
          server_events.handle_event(payload, trackers_server_socket, address)

      if verbose:
        print(f"[INFO] Client Address:\n{address}\n")
        print("[INFO] message_json:")
        pprint.pprint(payload)

    except socket.timeout:
      print("[DISCONNECTED] Tracker/Server socket timeout")

    except ConnectionResetError:
      print("[DISCONNECTED] Tracker/Server connection lost")



def communicate_driver_udp():
  # ************
  # Binds the Driver-Server socket with the designated port.
  # Begins communications with the OpenVR Driver.
  # ************

  global listening
  global driver_found

  while(listening):
    try:
      data, addr = driver_server_socket.recvfrom(BUFFER_SIZE)
      payload_length = len(data)

      if (payload_length == 3):
        header, msg, footer = unpack("=cbc", data)

        if (header == b'P' and footer == b'P'):
          driver_found = True
          addresses["openvr"] = addr
          payload = pack("=cbc", b'P', 45, b'p')
          driver_server_socket.sendto(payload, addr)

      elif (payload_length == 31):
        header, x, y, z, qw, qx, qy, qz, id, footer = unpack("=cfffffffbc", data)

        if (header == b'H' and footer == b'h'):
          hmd_pos = quaternion(0, x, y, z)
          hmd_quat = quaternion(qw, qx, qy, qz)

          if (calibrate):
            set_offsets (kinematics, trackers, hmd_quat, verbose=False)
            calibrate = False

          else:
            update_body(kinematics, trackers, hmd_pos, hmd_quat, verbose=False, offset_front=False, static_pos=False)

          for t in trackers.values():
            id = t.id
            quat = t.quat
            pos = t.pos

            payload = pack('=cfffffffbc', b'T', pos.x, pos.y, pos.z, quat.w, quat.x, quat.y, quat.z, id, b't')

            if addresses['openvr']:
              driver_server_socket.sendto(payload, addresses['openvr'])
    except socket.timeout:
      print("[DISCONNECTED] Driver/Server socket timeout")

    except ConnectionResetError:
      print("[DISCONNECTED] Driver/Server connection lost")

def sigint_handler(signum, frame):
  global listening
  global run_server

  print("Ctrl+C was entered!")
  listening = False
  run_server = False
  exit(0)

if __name__ == "__main__":
  parser = argparse.ArgumentParser()
  parser.add_argument('-v', type=bool)

  a = parser.parse_args()

  signal.signal(signal.SIGINT, sigint_handler)

  gui_udp = threading.Thread(target=communicate_gui_udp, args=(a.v,), daemon=True)
  gui_udp.start()

  tracker_udp = threading.Thread(target=communicate_trackers_udp, args=(a,), daemon=True)
  # tracker_udp.start()

  driver_udp =threading.Thread(target=communicate_driver_udp, daemon=True)
  # driver_udp.start()

  while(run_server):
    update_app(1, gui_server_socket, trackers, addresses['electron'])


  gui_udp.join()
  # driver_udp.join()
  tracker_udp.join()
