import serial
import socket


def setNetwork(port, ssid, password, ip_bytes, net_port):

    print("Setting network credentials")
    send_str = ssid.encode() + b'\x0A' + password.encode() + b'\x0A' + ip_bytes + b'\x0A' + net_port + b'\x0A'
    print(send_str)

    port.write(send_str)


    s = port.readline()
    print("SSID set as: ", s)
    p = port.readline()
    print("Password set as: ", p)
    ip_ret = port.readline()
    print(len(ip_ret))
    print("IP set as: ", ip_ret)
    port_ret = port.readline()
    print(len(port_ret))
    print("Port set as: ", port_ret)


# to do: if no write back with correct response, then assume that the ssid and password did not get sent, and resend it.

print("Hi!")

ser = serial.Serial("COM4", 115200, timeout = 1)

ssid = input("Enter SSID: ")
password = input("Enter password: ")
ip = input("Enter IP: ")
ip_bytes = socket.inet_aton(ip)

port = input("Enter port: ")
port = int(port)
port = port.to_bytes(2, 'big')

setNetwork(ser, ssid, password, ip_bytes, port)
#ser.close()

# reads info from the mcu
count = 0
while 1:
    count = count + 1