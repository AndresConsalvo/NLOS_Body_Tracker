//foundational code for establihing wifi connection and begining data transfer

#include <Arduino.h>
#include "io.h"
#include <ESP8266WiFi.h>
#include <WiFiUdp.h>
#include "wifi_connect.h"
#define WIFI_NETWORK "Jone"
#define WIFI_PASSWORD "freefolk"

#define WIFI_NETWORK "my network name goes here"
#define WIFI_PASSWORD "password here"

WiFiEventHandler wifiConnectedHandler;
WiFiEventHandler wifiDisconnectedHandler;

void wifiConnect() {
  io_init();
  //disconnect
  WiFi.disconnect(true);
  wifiConnectedHandler = WiFi.onStationModeConnected(&onConnect);
  wifiDisconnectedHandler = WiFi.onStationModeDisconnected(&onDisconnect);
  //pick station mode to connect to existing network
  //idk if we need this
  //WiFi.mode(WIFI_STA);
  //event handler
  //begin wifi wifi connection
  WiFi.mode(WIFI_STA);
  const char* ssid     = WIFI_NETWORK;
  const char* password = WIFI_PASSWORD;
  WiFi.begin(ssid, password);
  Serial.println("Beginning connection");
}


void onConnect(const WiFiEventStationModeConnected& event) {
  Serial.println("We connected bby");
}

void onDisconnect(const WiFiEventStationModeDisconnected& event){
  Serial.println("We not connected bby");
}
