#include <WiFi.h>
#include <WiFiUdp.h>
#include <stdio.h>
#include "defines.h"
#include "udp.h"
#include "imu.h"

const unsigned int udpPort = atoi(UDP_PORT);      // local port to listen on
char * udpAddress = UDP_ADDRESS;
WiFiUDP Udp;

void udp_init(){
  Udp.begin(udpPort);
}

void test_send(){
  // send a reply, to the IP address and port that sent us the packet we received
  char  ReplyBuffer[] = "This is a test of your ESP8266 services\r\n";       // a string to send back
  Udp.beginPacket(udpAddress, udpPort);
  Udp.printf(ReplyBuffer);
  Udp.endPacket();
  Serial.println("Packet sent");
}

void data_send(){
  /*
   *{
   *"type" : "POSITION",
   *"data" : {
   *"accel" : [x,y,z],
   *"gyro" : [x,y,z]
   *}
   */
  sensor_data s = getMpuValues();
  char ReplyBuffer[100];
  char ReplyBuffer2[100];
  char ReplyBuffer3[100];
  
  char  textBuffer[] = "\"type\" : \"POSITION\",\n\"data\" : {\n\"accel\" : [";
  char commaBuffer[] = ",";
  sprintf(ReplyBuffer, "%s%.2f%s%.2f%s%.2f%s %.2f%s%.2f%s%.2f%s", textBuffer, s.accel_x, ",", s.accel_y, ",", s.accel_z, "],\n\"gyro\" : [", s.gyro_x, ",", s.gyro_y, ",", s.gyro_z, "]\n}");

  
  Udp.beginPacket(udpAddress, udpPort);
  Udp.printf(ReplyBuffer);
  Udp.endPacket();
  Serial.println("Packet sent");
  Serial.println(ReplyBuffer);
}
