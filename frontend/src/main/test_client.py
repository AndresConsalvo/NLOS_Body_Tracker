import socket
import timeit
import random
import json
import sys

from tracker import Tracker

# from zmq import SERVER
# Generate random seeded data

SERVER_ADDR_PORT = ("127.0.0.1", 20001)
BUFFER_SIZE      = 1024
IP = socket.gethostbyname(socket.gethostname())

def generate_accel():
  return [random.random(), random.random(), random.random()]

def generate_gyro():
  return [random.randint(0,359), random.randint(0,359), random.randint(0,359)]

def generate_voltage():
  return random.random()+3

def handle_client(id:int, verbose=False):
  if verbose:
    print(f"[NEW CLIENT] Tracker-{id} connecting...")

  sock = socket.socket(family=socket.AF_INET, type=socket.SOCK_DGRAM,)
  voltage = generate_voltage()
  print(voltage)
  tracker = Tracker(IP, generate_accel(), generate_gyro(), voltage, "Tracker-"+str(id))
  message = json.dumps(tracker.get_device())
  bytes_to_send = str.encode(message)
  sock.sendto(bytes_to_send,SERVER_ADDR_PORT)

def start_test(n_messages=1, n_trackers=1, verbose=False):
  start = timeit.timeit()
  for m in range(0,n_messages):
    for i in range(0,n_trackers):
      handle_client(i+1,verbose)
  end = timeit.timeit()

  if verbose:
    print(f"[INFO] Time ellapsed to send {n_messages} from {n_trackers}:\n{end-start}")


if __name__ == "__main__":
  v = eval(sys.argv[1])
  start_test(verbose=v)
