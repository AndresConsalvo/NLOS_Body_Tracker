//foundational code for establihing wifi connection and begining data transfer

#include <Arduino.h>
#include "io.h"
#include "udp.h"
#include <ESP8266WiFi.h>
#include <WiFiUdp.h>
#include "wifi_connect.h"
#define WIFI_NETWORK "BetterSideOfTheTraplex"
#define WIFI_PASSWORD "Swamp9000"

WiFiEventHandler wifiConnectedHandler;
WiFiEventHandler wifiDisconnectedHandler;
String local_ip;

void wifiConnect() {
  io_init();
  //disconnect
  WiFi.disconnect(true);
  wifiConnectedHandler = WiFi.onStationModeConnected(&onConnect);
  wifiDisconnectedHandler = WiFi.onStationModeDisconnected(&onDisconnect);
  const char* ssid     = WIFI_NETWORK;
  const char* password = WIFI_PASSWORD;
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
    }
//  local_ip = WiFi.localIP();
  udp_init();
}


void onConnect(const WiFiEventStationModeConnected& event) {
  Serial.println("WiFi connected");
  Serial.print("IP address: ");
//  Serial.println(local_ip);
}

void onDisconnect(const WiFiEventStationModeDisconnected& event){
  Serial.println("connection failed");
}
