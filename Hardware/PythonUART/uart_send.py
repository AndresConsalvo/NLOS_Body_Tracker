import serial
import socket


def setNetwork(ser, ssid, password):
    try:
       
        print("Setting network credentials")
        send_str = ssid.encode() + password.encode()
        print(send_str)
        
        
        ser.write(send_str)

        ser.close()
    except serial.SerialException:
        print("COM port not connected")




# to do: if no write back with correct response, then assume that the ssid and password did not get sent, and resend it.

print("Hi!")
ser = serial.Serial("COM4", 115200, timeout = 1)
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

setNetwork(ser, ssid, password)

