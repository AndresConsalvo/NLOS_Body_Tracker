#include <Arduino.h>
#include "defines.h"
#include "io.h"
#include "udp.h"
#include <WiFi.h>
#include <WiFiUdp.h>
#include "wifi_connect.h"


//WiFiEventHandler wifiConnectedHandler;
//WiFiEventHandler wifiDisconnectedHandler;
String local_ip;

void wifiConnect() {
  io_init();
  const char* ssid     = WIFI_NETWORK;
  const char* password = WIFI_PASSWORD;
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  Serial.print("Connecting to ");
  Serial.println(WIFI_NETWORK);
  udp_init();
}

bool getWifiStatus(){
  return (WiFi.status() == WL_CONNECTED) ? 1 : 0;
}
