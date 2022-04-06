import serial
import socket


def setNetwork(port, ssid, password, ip_bytes, tracker_id):
    try:
        ser = serial.Serial("COM4", 115200, timeout = 1)
        print("Setting network credentials")
        send_str = ssid.encode() + b'\x0A' + password.encode() + b'\x0A' + ip_bytes + b'\x0A' + tracker_id + b'\x0A'
        print(send_str)
        
        
        ser.write(send_str)
        sid = ser.readline()
        print(sid)
        pss = ser.readline()
        print(pss)
        ip = ser.readline()
        print(ip)
        id = ser.readline()
        print(id)

        ser.close()
    except serial.SerialException:
        print("COM port not connected")




# to do: if no write back with correct response, then assume that the ssid and password did not get sent, and resend it.

print("Hi!")

COM = input("Enter COM port: ")

ssid = input("Enter SSID: ")
password = input("Enter password: ")
ip = input("Enter IP: ")
#ssid = "uam8nsw9dt9q"
#password = "K-jRAw9YC5-U"
#ip = "192.168.1.59"

ip_bytes = socket.inet_aton(ip)


id = input("Enter id: ")
#id = "2"
id = int(id)
id = id.to_bytes(1, 'big')

setNetwork(COM, ssid, password, ip_bytes, id)

