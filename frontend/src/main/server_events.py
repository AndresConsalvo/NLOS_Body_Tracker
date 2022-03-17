import socket
import json
import pprint
import struct
from dotenv import load_dotenv
from tracker import Tracker

load_dotenv()

initial_state = {"trackers":{},
                 "address":(),
                 "electron_adr":(),
                 "openvr_adr":()}

state = dict(initial_state)

def dispatch(payload:dict) -> None:
  global state
  state  = root_reducer(payload, state)

def get_state():
  global state
  return state

store = {"dispatch":dispatch,
         "get_state":get_state}

def root_reducer(action, state:dict=initial_state) -> dict:
  pass
class ServerEvent:
  def __init__(self, trackers:dict[str,Tracker]=None,) -> None:
    self.addresses = {
      "openvr": None,
      "electron": None
    }
    self.body_measurements = {
      "ankleToGround": None,
      "headToNeck": None,
      "neckToWaist": None,
      "waistToAnkle": None,
    }
    self.trackers = trackers

  def set_payload(self, payload:dict) -> None:
    self.payload = payload

  def handle_event(self, payload, *args) -> None:
    if (payload["type"] == "DEVICE"):
      print('[EVENT] device')
      self.device_event(payload["data"], args[0])

    elif (payload["type"] == "DEVICE_STATS"):
      print('[EVENT] device stats')
      pass
    elif (payload["type"] == "ELECTRON_HAND_SHAKE"):
      print('[EVENT] electron handshake')
      self.electron_handshake_event(args[1])

    elif (payload["type"] == "CHANGE_ROLE"):
      print('[EVENT] change role')
      self.change_role_event(payload["data"])

    elif (payload["type"] == "CALIBRATE"):
      print('[EVENT] calibrate')
      self.calibrate_event(args[0])

    elif (payload["type"] == "POSITION"):
      print('[EVENT] position')
      self.position_event(payload["data"], args[0])
      pass

    elif (payload["type"] == "BODY_MEASUREMENTS"):
      print('[EVENT] BODY_MEASUREMENTS')
      self.body_measurements_event(payload["data"], args[0])

  def device_event(self,data, UDP_server_socket:socket.socket):
    tracker = Tracker(data["ip"],
                      data["accel"],
                      data["gyro"],
                      4.1,
                      data["id"],
                      data["body_part"])

    if not (tracker.id in self.trackers):
      self.trackers[tracker.id] = tracker
    else:
      self.trackers[tracker.id].update(id       =tracker.id,
                                       ip       =tracker.ip,
                                       accel    =tracker.accel,
                                       gyro     =tracker.gyro,
                                       battery  =tracker.battery,
                                       )

    if self.addresses["electron"]:
      message_to_send = json.dumps(self.trackers[tracker.id].get_device())
      bytes_to_send = str.encode(message_to_send)
      UDP_server_socket.sendto(bytes_to_send, self.addresses["electron"])

  def device_stats_event(self):
    pass

  def electron_handshake_event(self, address):
    self.addresses["electron"] = address

  def change_role_event(self, data):
    self.trackers[data["id"]].update(body_part=data["body_part"])

  def calibrate_event(self, UDP_server_socket:socket.socket):
    for t in self.trackers.values():
      t.update(gyro=[0,0,0])
      if self.addresses["openvr"]:
        message_to_send = t.gyro.append(float(t.id))
        bytes_to_send = struct.pack('%sf' % len(message_to_send), *message_to_send)
        UDP_server_socket.sendto(bytes_to_send, self.addresses["openvr"])

  def position_event(self, args):

    UDP_server_socket:socket.socket = args[0]

    accel = self.data["accel"]
    gyro  = self.data["gyro"]
    id    = self.data["id"]

    if not (id in self.trackers):
      tracker = Tracker(self.data["ip"],
                        accel,
                        gyro,
                        4.2,
                        id,
                        None)

      self.trackers[id]= tracker

    else:
      self.trackers[id].update(gyro=gyro,accel=accel)

    if self.addresses["openvr"]:
      message_to_send = gyro.append(float(tracker.id))
      bytes_to_send = struct.pack('%sf' % len(message_to_send), *message_to_send)
      UDP_server_socket.sendto(bytes_to_send, self.addresses["openvr"])

  def body_measurements_event(self, data, UDP_server_socket):
    self.body_measurements = {
      "ankleToGround": data["ankleToGround"]["value"],
      "headToNeck": data["headToNeck"]["value"],
      "neckToWaist": data["neckToWaist"]["value"],
      "waistToAnkle": data["waistToAnkle"]["value"],
    }

    if self.addresses["openvr"]:
      print('[SENDING] body measurements to openvr')
      message_to_send = json.dumps(self.body_measurements)
      bytes_to_send = str.encode(message_to_send)
      UDP_server_socket.sendto(bytes_to_send, self.addresses["openvr"])

