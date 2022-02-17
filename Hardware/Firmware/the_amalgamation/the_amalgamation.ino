#include <Arduino.h>
#include "wifi_connect.h"
#include "io.h"
#define WIFI_NETWORK "Jone"
#define WIFI_PASSWORD "freefolk"

//void set
//Serial.println("Beginning...")
//
//wifiConnect();

void setup() {
  Serial.begin(115200);
  io_init();
  wifiConnect();
}

void loop() {
  // put your main code here, to run repeatedly:
//  Serial.println("LEts go");
  signalSOS();
}



//WiFiEventHandler wifiConnectedHandler;
//WiFiEventHandler wifiDisconnectedHandler;
//
//
//void wifiConnect() {
//  io_init();
//  //disconnect
//  WiFi.disconnect(true);
//  wifiConnectedHandler = WiFi.onStationModeConnected(&onConnect);
//  wifiDisconnectedHandler = WiFi.onStationModeDisconnected(&onDisconnect);
//  //pick station mode to connect to existing network
//  //idk if we need this
//  //WiFi.mode(WIFI_STA);
//  //event handler
//  //begin wifi wifi connection
//  WiFi.mode(WIFI_STA);
//  const char* ssid     = WIFI_NETWORK;
//  const char* password = WIFI_PASSWORD;
//  WiFi.begin(ssid, password);
//  Serial.println("Beginning connection");
//}
//
//
//void onConnect(const WiFiEventStationModeConnected& event) {
//  Serial.println("We connected bby");
//}
//
//void onDisconnect(const WiFiEventStationModeDisconnected& event){
//  Serial.println("We not connected bby");
//}
