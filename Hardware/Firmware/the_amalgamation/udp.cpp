#include <WiFi.h>
#include <WiFiUdp.h>
#include "udp.h"

unsigned int udpPort = 20001;      // local port to listen on
const char * udpAddress = "192.168.0.150";
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
