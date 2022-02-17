#include <ESP8266WiFi.h>

void onDisconnect(const WiFiEventStationModeDisconnected& event);
void onConnect(const WiFiEventStationModeConnected& event);
void wifiConnect();
void wifi_init();
