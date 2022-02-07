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
    public float Gx, Gy, Gz = 0;
    public float Ax, Ay, Az = 0;

    public float x, y, z = 0;

    IPEndPoint sender;
    UdpClient newsock; 
    byte[] data = new byte[12];
    Thread thread;

    // Start is called before the first frame update
    void Start() {
        Application.targetFrameRate = 60;
        sender = new IPEndPoint(IPAddress.Any, 20000);
        newsock = new UdpClient(new IPEndPoint(IPAddress.Any, 20000));
        newsock.Client.ReceiveTimeout = 1000;
        thread = new Thread(new ThreadStart(UDPReceive));
        thread.Start();
    }

    // Update is called once per frame
    void Update() {
        if (Input.GetKeyDown("space")) {
            print("space key was pressed");


            transform.localEulerAngles = new Vector3(0, 0, 0);
        }

        Gx = (float) ((Int16)(data[0] << 8 | data[1]));
        Gy = (float) ((Int16)(data[2] << 8 | data[3]));
        Gz = (float) ((Int16)(data[4] << 8 | data[5]));


        x = Gy / 675;
        y = Gz / 675;
        z = Gx / 675;

        //print(x);
        float radius = 50;
        transform.Rotate(new Vector3(-x, y, -z));

        Vector3 forwardVector = transform.rotation * -Vector3.forward;
        Vector3 newpos = new Vector3(radius * forwardVector.x, radius * forwardVector.y, radius * forwardVector.z);

        transform.position = newpos;

    }

    private void UDPReceive() {
        while (true) {
            try {
                data = newsock.Receive(ref sender);
            } catch (Exception ex) {
                //newsock = new UdpClient(new IPEndPoint(IPAddress.Any, 90));
                print("Not received");
            }

        }
    }

    public void OnApplicationQuit() {
        /*        print("Port 5 is closed!");
                serial.Close();*/
        newsock.Close();
        thread.Abort();
    }

    private void StartUDPServer() {

    }
}
