import socket
import json
import pprint
import struct
from dotenv import load_dotenv
from frontend.src.main.math_helpers import *
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
      self.position_event(payload["data"])
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

    # if self.addresses["electron"]:
    #   message_to_send = json.dumps(self.trackers[tracker.id].get_device())
    #   bytes_to_send = str.encode(message_to_send)
    #   UDP_server_socket.sendto(bytes_to_send, self.addresses["electron"])

  def device_stats_event(self):
    pass

  def electron_handshake_event(self, address):
    self.addresses["electron"] = address

  def change_role_event(self, data):
    bodyparts = {
      "Chest":1,
      "Waist":2,
      "Left Knee":3,
      "Right Knee":4,
      "Left Ankle":5,
      "Right Ankle":6,
    }
    self.trackers[data["id"]].update(body_part=bodyparts[data["body_part"]])

  def calibrate_event(self, UDP_server_socket:socket.socket):
    for t in self.trackers.values():
      t.update(gyro=[0,0,0])
      if self.addresses["openvr"]:
        message_to_send = t.gyro.append(4)
        bytes_to_send = struct.pack('%sf' % len(message_to_send), *message_to_send)
        UDP_server_socket.sendto(bytes_to_send, self.addresses["openvr"])

  def position_event(self, data):

    accel = data["accel"]
    gyro  = data["gyro"]
    id    = int(data["id"])
    quat  = data["quat"]

    tracker = Tracker("127.0.0.1",
                      accel,
                      gyro,
                      4.2,
                      id,
                      None)

    if not (tracker.id in self.trackers):
      self.trackers[tracker.id] = tracker
      self.trackers[tracker.id].quat_from_imu = quaternion(quat[0], quat[1], quat[2], quat[3])

    else:
      self.trackers[tracker.id].update(id       =tracker.id,
                                       ip       =tracker.ip,
                                       accel    =tracker.accel,
                                       gyro     =tracker.gyro,
                                       battery  =tracker.battery,
                                       )
      self.trackers[tracker.id].quat_from_imu = quaternion(quat[0], quat[1], quat[2], quat[3])


  def body_measurements_event(self, data, UDP_server_socket):
    self.body_measurements = {
      "ankleToGround": data["ankleToGround"]["value"],
      "headToNeck": data["headToNeck"]["value"],
      "neckToWaist": data["neckToWaist"]["value"],
      "waistToAnkle": data["waistToAnkle"]["value"],
    }

    message_to_send = ['a',
                        float(self.body_measurements["ankleToGround"]),
                        float(self.body_measurements["headToNeck"]),
                        float(self.body_measurements["neckToWaist"]),
                        float(self.body_measurements["waistToAnkle"])
                        ]

    print('[MESSAGE_TO_SEND]',message_to_send)
    if self.addresses["openvr"]:
      print('[SENDING] body measurements to openvr')
      bytes_to_send = str.encode(message_to_send)
      UDP_server_socket.sendto(bytes_to_send, self.addresses["openvr"])

