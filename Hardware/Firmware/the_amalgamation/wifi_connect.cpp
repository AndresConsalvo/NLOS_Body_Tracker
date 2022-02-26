#include <Arduino.h>
#include "io.h"
#include "udp.h"
#include <WiFi.h>
#include <WiFiUdp.h>
#include "wifi_connect.h"
#define WIFI_NETWORK "BetterSideOfTheTraplex"
#define WIFI_PASSWORD "Swamp9000"

//WiFiEventHandler wifiConnectedHandler;
//WiFiEventHandler wifiDisconnectedHandler;
String local_ip;

void wifiConnect() {
  io_init();
  const char* ssid     = WIFI_NETWORK;
  const char* password = WIFI_PASSWORD;
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
//  WiFi.onEvent(onConnect, SYSTEM_EVENT_STA_CONNECTED);
//  WiFi.onEvent(onDisconnect, SYSTEM_EVENT_STA_DISCONNECTED);
  Serial.print("Connecting to ");
  Serial.println(WIFI_NETWORK);
//   while (WiFi.status() != WL_CONNECTED) {
//    Serial.print('.');
//    delay(500);
//  }
//  local_ip = WiFi.localIP();
  udp_init();
}

bool getWifiStatus(){
  return (WiFi.status() == WL_CONNECTED) ? 1 : 0;
}


//void wifiConnect() {
//  io_init();
//  //disconnect
//  WiFi.disconnect(true);
//  wifiConnectedHandler = WiFi.onStationModeConnected(&onConnect);
//  wifiDisconnectedHandler = WiFi.onStationModeDisconnected(&onDisconnect);
//  const char* ssid     = WIFI_NETWORK;
//  const char* password = WIFI_PASSWORD;
//  WiFi.mode(WIFI_STA);
//  WiFi.begin(ssid, password);
//
//  while (WiFi.status() != WL_CONNECTED) {
//    delay(500);
//    Serial.print(".");
//    }
////  local_ip = WiFi.localIP();
//  udp_init();
//}


void onConnect(WiFiEvent_t event, WiFiEventInfo_t info) {
  Serial.println("WiFi connected");
  Serial.print("IP address: ");
//  Serial.println(local_ip);
}

void onDisconnect(WiFiEvent_t event, WiFiEventInfo_t info){
  Serial.println("connection failed");
}
