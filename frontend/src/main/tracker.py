import math

MAX_VOLTAGE = 4.2
MIN_VOLTAGE = 3.0


class Tracker:

  def __init__(self, ip, accel:list, gyro:list, battery:float, id=None, body_part=None) -> None:
    self.id = id
    self.ip = ip
    self.accel = accel
    self.gyro = gyro
    self.battery = self.compute_battery_percent(battery)
    self.body_part = body_part

  def get_data(self) -> dict:
    data = {
      "type" : 'TRACKER_POSITION',
      "data" : {
        "id" : self.id,
        "accel" : self.accel,
        "gyro" : self.gyro
      }
    }
    return data

  def get_device_stats(self) -> dict:
    stats = {
      "type" : 'DEVICE_STATS',
      "data" : {
        "id" : self.id,
        "ip" : self.ip,
        "battery" : self.battery,
        "body_part" : self.body_part
      }
    }
    return stats

  def get_device(self) -> dict:
    device = {
      "type" : 'DEVICE',
      "data" : {
        "id" : self.id,
        "ip" : self.ip,
        "accel" : self.accel,
        "gyro" : self.gyro,
        "battery" : self.battery,
        "body_part" : self.body_part
      }
    }
    return device

  def set_body_part(self, bodypart):
    self.body_part = bodypart

  def set_battery(self, voltage):
    self.battery = self.compute_battery_percent(voltage)

  def compute_battery_percent(self,voltage):
    return math.floor(((MAX_VOLTAGE - voltage)/(MAX_VOLTAGE - MIN_VOLTAGE))* 100)

  def refresh(self, id=None, ip=None, accel=None, gyro=None, battery=None, body_part=None):
    if id is not None:
      self.id = id
    if ip is not None:
      self.ip = ip
    if accel is not None:
      self.accel = accel
    if gyro is not None:
      self.gyro = gyro
    if battery is not None:
      self.battery = self.compute_battery_percent(battery)
    if body_part is not None:
      self.body_part = body_part
