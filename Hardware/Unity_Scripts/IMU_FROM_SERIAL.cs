using System;
using System.Collections;
using System.Collections.Generic;
using System.IO.Ports;
using System.Threading;
using System.Net;
using System.Net.Sockets;
using System.Text;
using UnityEngine;

public class IMU_FROM_SERIAL : MonoBehaviour {
    public static SerialPort serial = new SerialPort("COM6", 115200);

    byte[] data = { 0, 0, 0, 0, 0, 0 };
    Vector3 v3 = Vector3.zero;
    public float x, y, z = 0;
    public float pos_x, pos_y, pos_z = 0;
    public float vel_x, vel_y, vel_z = 0;


    // Start is called before the first frame update
    void Start() {
        Application.targetFrameRate = 60;
        if (serial.IsOpen == false) {
            print("Opened port 5 at rate 115200!");
            serial.Open();
            serial.ReadTimeout = 2000;
        }
    }

    // Update is called once per frame
    void Update() {
        if (Input.GetKeyDown("space")) {
            print("space key was pressed");

            transform.localEulerAngles = new Vector3(0, 0, 0);

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
        float Az = ((float)int.Parse(serial.ReadLine()) / (float)16384.0 + (float)1.0) * (float)9.84;


        //print(Gx);
        //print(Gy);
        //print(Gz);
        //print(Ax);
        //print(Ay);
        //print(Az);

        x = Gy / 675;
        y  = Gz / 675;
        z = Gx / 675;

        //print(x);
        float radius = 50;
        transform.Rotate(new Vector3(x, y, z));

        //Vector3 forwardVector = transform.rotation * -Vector3.forward;
        //Vector3 newpos = new Vector3(-radius * forwardVector.x, -radius * forwardVector.y, -radius * forwardVector.z);

        //transform.position = newpos;

        /*        float half = (float) 0.5;
        transform.position = new Vector3(new_pos_x, new_pos_y, new_pos_z);*/
        /*
                float t = Time.deltaTime;
                Vector3 curr_pos = transform.position;
                float new_pos_x = curr_pos.x + vel_x * t + half * Ay * t * t;
                float new_pos_y = curr_pos.z + vel_z * t + half * (-Az) * t * t;
                float new_pos_z = curr_pos.x + vel_x * t + half * Ax * t * t;*/

        //vel_x += (Ay * Time.deltaTime);
        //vel_y += (-Az * Time.deltaTime);
        //vel_z += (Ax * Time.deltaTime);
        //print(pos_x);
        //print(pos_y);
        //print(pos_z);



        //Debug.Log(transform.position);

        // Perform normalization, add to previous frame
        // Apply to gameobject

        //v3.x += 4 * Time.deltaTime;
        //v3.y += 4 * Time.deltaTime;
        //v3.z += 4 * Time.deltaTime;


    }

    public void OnApplicationQuit() {
        print("Port 5 is closed!");
        serial.Close();
    }
}

