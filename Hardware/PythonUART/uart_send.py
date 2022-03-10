import serial


def setNetwork(port, ssid, password):

    print("Setting network credentials")
    send_string = ssid + '\n' + password + '\n'
    send_string = send_string.encode()

    port.write(send_string)


    s = port.readline()
    print("SSID set as: ", s)
    p = port.readline()
    print("Password set as: ", p)

# to do: if no write back with correct response, then assume that the ssid and password did not get sent, and resend it.

print("Hi!")

ser = serial.Serial('COM4', 115200, timeout = 1)
ser.is_open

ssid = input("Enter SSID: ")
password = input("Enter password: ")

setNetwork(ser, ssid, password)

# reads info from the mcu
while 1:
    ret = ser.readline()
    print(ret)