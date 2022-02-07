using System.Collections;
using System.Collections.Generic;
using System.IO.Ports;
using UnityEngine;

public class Accel_Test : MonoBehaviour {
    public static SerialPort serial = new SerialPort("COM5", 9600);
    
    private bool LEDState = true;

    public void OnMouseDown()
    {
        print("Clicked");
        if(serial.IsOpen == false)
        {
            serial.Open();
        }

        if (LEDState == false)
        {
            serial.Write("A");
            LEDState = true;
        } else
        {
            serial.Write("B");
            LEDState = false;
        }
    }

    public void OnApplicationClose()
    {
        serial.Close();
    }

}


