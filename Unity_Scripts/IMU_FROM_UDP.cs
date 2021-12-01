using System;
using System.Collections;
using System.Collections.Generic;
using System.IO.Ports;
using System.Threading;
using System.Net;
using System.Net.Sockets;
using System.Text;
using UnityEngine;

public class IMU_FROM_UDP : MonoBehaviour {
    //public static SerialPort serial = new SerialPort("COM6", 115200);

    Vector3 v3 = Vector3.zero;
    public float x, y, z = 0;
    public float pos_x, pos_y, pos_z = 0;
    public float vel_x, vel_y, vel_z = 0;

    IPEndPoint sender;
    UdpClient newsock; 
    byte[] data = new byte[1024];

    // Start is called before the first frame update
    void Start() {
        Application.targetFrameRate = 60;
        /*        if (serial.IsOpen == false) {
                    print("Opened port 5 at rate 115200!");
                    serial.Open();
                    serial.ReadTimeout = 2000;
                }*/

        sender = new IPEndPoint(IPAddress.Any, 20000);
        newsock = new UdpClient(new IPEndPoint(IPAddress.Any, 20000));
        newsock.Client.ReceiveTimeout = 1000;



    }

    // Update is called once per frame
    void Update() {
        try {
            data = newsock.Receive(ref sender);
            string hex = BitConverter.ToString(data);
            print(hex);
        } catch (Exception ex) {
            //newsock = new UdpClient(new IPEndPoint(IPAddress.Any, 90));
            print("Not received");
        }

    }

    public void OnApplicationQuit() {
        /*        print("Port 5 is closed!");
                serial.Close();*/
        newsock.Close();
    }

    private void StartUDPServer() {

    }
}
