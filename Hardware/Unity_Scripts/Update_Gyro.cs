using System;
using System.Collections;
using System.Collections.Generic;
using System.IO.Ports;
using System.Threading;
using System.Net;
using System.Net.Sockets;
using System.Text;
using UnityEngine;

public class Update_Gyro : MonoBehaviour {
    //public static SerialPort serial = new SerialPort("COM6", 115200);

    byte[] data_imu = { 0, 0, 0, 0, 0, 0 };
    //Vector3 v3 = Vector3.zero;
    public float x, y, z = 0;
    public float pos_x, pos_y, pos_z = 0;
    public float vel_x, vel_y, vel_z = 0;

    

    // Start is called before the first frame update
    void Start() {
        Application.targetFrameRate = 60;
/*        if (serial.IsOpen == false) {
            print("Opened port 5 at rate 115200!");
            serial.Open();
            serial.ReadTimeout = 2000;
        }*/

        byte[] data = new byte[1024];
        IPEndPoint ipep = new IPEndPoint(IPAddress.Any, 9050);
        IPEndPoint sender = new IPEndPoint(IPAddress.Any, 0);
        UdpClient newsock = new UdpClient(ipep);

        data = newsock.Receive(ref sender);
        string s1 = Encoding.UTF8.GetString(data);
        print(s1);
        Console.WriteLine("Message received from {0}: ", sender.ToString());
        Console.WriteLine(Encoding.ASCII.GetString(data,0,data.Length));
    }

    // Update is called once per frame
    void Update() {
/*        if (Input.GetKeyDown("space")) {
            print("space key was pressed");
            x = 0;
            y = 0;
            z = 0;

            transform.localPosition = new Vector3(0, 0, 0);
        }
        // Request data from IMU
        serial.Write("A");
        float Gx = int.Parse(serial.ReadLine()) + 1;
        serial.Write("B");
        float Gy = int.Parse(serial.ReadLine()) + 2;
        serial.Write("C");
        float Gz = int.Parse(serial.ReadLine()) - 7;
        serial.Write("D");
        float Ax = ((float)int.Parse(serial.ReadLine()) / (float)16384.0) * (float)9.84;
        serial.Write("E");
        float Ay = ((float)int.Parse(serial.ReadLine()) / (float)16384.0) * (float)9.84;
        serial.Write("F");
        float Az = ((float) int.Parse(serial.ReadLine()) / (float)16384.0 + (float)1.0) * (float)9.84; 


        //print(Gx);
        //print(Gy);
        //print(Gz);
        //print(Ax);
        //print(Ay);
        print(Az);

        x += -Gy / 750;
        y += Gz / 750;
        z += -Gx / 750;
        
        transform.localEulerAngles = new Vector3(x, y, z);
        //Debug.Log(transform.localEulerAngles);

        float t = Time.deltaTime;

        vel_x += (Ay * Time.deltaTime);
        vel_y += (-Az * Time.deltaTime);
        vel_z += (Ax * Time.deltaTime);
        //print(pos_x);
        //print(pos_y);
        //print(pos_z);
        Vector3 curr_pos = transform.localPosition;
        transform.localPosition = new Vector3(curr_pos.x + vel_x * t + (1/2) * Ax * t * t, curr_pos.y + vel_y * t + (1/2) * Ay * t * t, curr_pos.z + vel_z * t + (1 / 2) * Az * t * t);

        //Debug.Log(transform.localPosition);

        // Perform normalization, add to previous frame
        // Apply to gameobject

        *//*        v3.x += 4 * Time.deltaTime;
                v3.y += 4 * Time.deltaTime;
                v3.z += 4 * Time.deltaTime;

                gameObject.transform.eulerAngles = v3;*/
    }

    public void OnApplicationQuit() {
/*        print("Port 5 is closed!");
        serial.Close();*/
    }

    private void StartUDPServer() {

    }
}
