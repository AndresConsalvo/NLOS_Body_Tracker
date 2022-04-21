import serial
import socket


def setNetwork(port, ssid, password):
    try:
        ser = serial.Serial(port, 115200, timeout = 1)
        print("Setting network credentials")
        send_str = ssid.encode() + b'\n' + password.encode() + b'\n'
        print(send_str)
        
        
        ser.write(send_str)

        ser.close()
    except serial.SerialException:
        print("COM port not connected")




# to do: if no write back with correct response, then assume that the ssid and password did not get sent, and resend it.

print("Hi!")
COM = input("Enter COM port: ")

#ssid = input("Enter SSID: ")
#password = input("Enter password: ")
#ip = input("Enter IP: ")
ssid = "uam8nsw9dt9q"
password = "K-jRAw9YC5-U"
#ip = "192.168.1.59"

#ip_bytes = socket.inet_aton(ip)


#id = input("Enter id: ")
#id = "2"
#id = int(id)
#id = id.to_bytes(1, 'big')

setNetwork(COM, ssid, password)

