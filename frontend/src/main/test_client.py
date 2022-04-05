import socket
import timeit
import random
import json
import sys
import time
import argparse

from tracker import Tracker

# from zmq import SERVER
# Generate random seeded data

SERVER_ADDR_PORT = ("192.168.1.51", 20000)
BUFFER_SIZE      = 1024
IP = socket.gethostbyname(socket.gethostname())

def generate_accel():
  return [random.random(), random.random(), random.random()]

def generate_gyro():
  return [random.randint(0,359), random.randint(0,359), random.randint(0,359)]

def generate_voltage():
  return random.random()+3

def handle_client(sock, id:int, verbose=False):
  if verbose:
    print(f"[INFO] Tracker-{id} connecting...")

  voltage = generate_voltage()
  tracker = Tracker(IP, generate_accel(), generate_gyro(), voltage, "Tracker-"+str(id))
  message = json.dumps(tracker.get_device())
  bytes_to_send = str.encode(message)
  sock.sendto(bytes_to_send,SERVER_ADDR_PORT)

def start_test(n_messages, n_trackers, verbose):
  if n_messages is None or not isinstance(n_messages, int):
    n_messages = 1

  if n_trackers is None or not isinstance(n_trackers, int):
    n_trackers = 1

  if verbose is None or not isinstance(verbose, bool):
    verbose = False

  start = timeit.timeit()
  sockets = []

  for i in range(0, n_trackers):
    sock = socket.socket(family=socket.AF_INET, type=socket.SOCK_DGRAM,)
    sockets += [sock]

  for m in range(0,n_messages):
    for i in range(0,n_trackers):
      handle_client(sockets[i], i+1, verbose)
      time.sleep(0.07)

  end = timeit.timeit()

  if verbose:
    print(f"[INFO] Time ellapsed to send {n_messages} from {n_trackers}:\n{end-start}")


if __name__ == "__main__":
  parser = argparse.ArgumentParser()
  parser.add_argument('-v', type=bool)
  parser.add_argument('-t', type=int)
  parser.add_argument('-m', type=int)
  args = parser.parse_args()



  start_test(args.m, args.t, args.v)
