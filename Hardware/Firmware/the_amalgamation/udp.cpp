#include <WiFi.h>
#include <WiFiUdp.h>
#include <stdio.h>
#include "defines.h"
#include "udp.h"
#include "imu.h"

const unsigned int udpPort = atoi(UDP_PORT);      
const char * udpAddress = UDP_ADDRESS;
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

/*
   *{
   *"type" : "POSITION",
   *"data" : {
   *"accel" : [x,y,z],
   *"gyro" : [x,y,z]
   *}
   */
byte buff[sizeof(float) * 6 + 1];

int packet_count = 100;
void data_send(){ 
  sensor_data s = getMpuValues();

  memcpy(buff,&s.accel_x,sizeof(float));
  memcpy(buff + sizeof(float) * 1, &s.accel_y, sizeof(float));
  memcpy(buff + sizeof(float) * 2, &s.accel_z, sizeof(float));
  memcpy(buff + sizeof(float) * 3, &s.gyro_x, sizeof(float));
  memcpy(buff + sizeof(float) * 4, &s.gyro_y, sizeof(float));
  memcpy(buff + sizeof(float) * 5, &s.gyro_z, sizeof(float));
  buff[sizeof(float) * 6] = 0;

  /*
  char ReplyBuffer[100];
  char ReplyBuffer2[100];
  char ReplyBuffer3[100];
  
  char  textBuffer[] = "\"type\" : \"POSITION\",\n\"data\" : {\n\"accel\" : [";
  char commaBuffer[] = ",";
  sprintf(ReplyBuffer, "%s%.2f%s%.2f%s%.2f%s %.2f%s%.2f%s%.2f%s", textBuffer, s.accel_x, ",", s.accel_y, ",", s.accel_z, "],\n\"gyro\" : [", s.gyro_x, ",", s.gyro_y, ",", s.gyro_z, "]\n}");
  */
  
  Udp.beginPacket(udpAddress, udpPort);
  //Udp.printf(ReplyBuffer);
  Udp.write(buff, sizeof(float) * 6 + 1);
  Udp.endPacket();
  if(packet_count == 200){
    //Serial.println(" 200 Packets sent");
    //Serial.println(ReplyBuffer);
    packet_count = 0;
  }
  packet_count += 1;
}
