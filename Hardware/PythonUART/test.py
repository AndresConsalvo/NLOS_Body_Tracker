import socket

broad_listen = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
broad_listen.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
broad_listen.bind(('0.0.0.0', 4242))
broad_listen.settimeout(0.5)

s = socket.socket(socket.AF_INET, socket.SOCK_DGRAM, 0)

payload, addr = broad_listen.recvfrom(32)
print(payload)
print(addr[0])

ip = addr[0]
test = b"hi"
send_len = s.sendto(test, (ip, 4242))

while True:
    print("Receiving")
    print(s.getsockname())
    payload2, addr2 = s.recvfrom(32)
    print(addr2)
    print(payload2)
