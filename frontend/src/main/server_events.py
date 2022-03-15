import socket
import json
import pprint
import struct
from dotenv import load_dotenv
from main.server import UDP_server_socket
from tracker import Tracker

load_dotenv()


class ServerEvent:
  def __init__(self, type:str, data:dict) -> None:
    self.type = type
    self.data = data
    self.electron_address = None
    self.open_vr_address = None

  def set_type(self, type:str) -> None:
    self.type = type

  def set_data(self, data) -> None:
    self.data = data

  def handle_event(self, *args) -> None:
    if (self.type == "DEVICE"):
      print('[EVENT] device')
      self.device_event(args[0],args[1])

    elif (self.type == "DEVICE_STATS"):
      print('[EVENT] device stats')
      pass
    elif (self.type == "ELECTRON_HAND_SHAKE"):
      print('[EVENT] electron handshake')
      pass
    elif (self.type == "CHANGE_ROLE"):
      print('[EVENT] change role')
      pass
    elif (self.type == "CALIBRATE"):
      print('[EVENT] calibrate')
      pass
    elif (self.type == "POSITION"):
      print('[EVENT] position')
      self.position_event(args[0],args[1])
      pass
    elif (self.type == "BODY_MEASUREMENTS"):
      print('[EVENT] BODY_MEASUREMENTS')
      pass

  def device_event(self, trackers:dict[str,Tracker], UDP_server_socket:socket.socket):
    tracker = Tracker(self.data["ip"],
                      self.data["accel"],
                      self.data["gyro"],
                      4.2,
                      self.data["id"],
                      self.data["body_part"])

    if not (tracker.id in trackers):
      trackers[tracker.id] = tracker
    else:
      trackers[tracker.id].update(id       =tracker.id,
                                  ip       =tracker.ip,
                                  accel    =tracker.accel,
                                  gyro     =tracker.gyro,
                                  battery  =tracker.battery,
                                  body_part=tracker.body_part)

      if self.electron_address:
        message_to_send = json.dumps(trackers[tracker.id].get_device())
        bytes_to_send = str.encode(message_to_send)
        UDP_server_socket.sendto(bytes_to_send, self.electron_address)

  def device_stats_event(self):
    pass

  def change_role_event(self, trackers:list):

    pass

  def calibrate_event(self):
    pass

  def position_event(self, trackers:dict[str,Tracker], UDP_server_socket:socket.socket):
    accel = self.data["accel"]
    gyro  = self.data["gyro"]
    id    = self.data["id"]

    if not (id in trackers):
      tracker = Tracker(self.data["ip"],
                        accel,
                        gyro,
                        4.2,
                        id,
                        None)

      trackers[id]= tracker

    else:
      trackers[id].update(gyro=gyro,accel=accel)

    if self.open_vr_address:
      message_to_send = gyro.append(float(tracker.id))
      bytes_to_send = struct.pack('%sf' % len(message_to_send), *message_to_send)
      UDP_server_socket.sendto(bytes_to_send, self.open_vr_address)


