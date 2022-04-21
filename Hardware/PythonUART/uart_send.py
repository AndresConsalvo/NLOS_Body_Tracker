import serial
import socket
import time


def setNetwork(ser, ssid, password):
    try:
        #ser = serial.Serial(port, 115200, timeout = 1)
        print("Setting network credentials")
        send_str = ssid.encode() + b'\n' + password.encode() + b'\n'
        print(send_str)
        
        

        ser.write(send_str)
        while 1:
            sid = ser.readline()
            print(sid)
        sid = ser.readline()
        print(sid)
        pss = ser.readline()
        print(pss)

        ser.close()
    except serial.SerialException:
        print("COM port not connected")




# to do: if no write back with correct response, then assume that the ssid and password did not get sent, and resend it.

print("Hi!")
#COM = input("Enter COM port: ")
ser = serial.Serial("COM3", 115200, timeout = 1)
time.sleep(5)

#ssid = input("Enter SSID: ")
#password = input("Enter password: ")
#ip = input("Enter IP: ")
ssid = "Oppai"
password = "hero4fun"
#ip = "192.168.1.59"

#ip_bytes = socket.inet_aton(ip)


#id = input("Enter id: ")
#id = "2"
#id = int(id)
#id = id.to_bytes(1, 'big')

setNetwork(ser, ssid, password)