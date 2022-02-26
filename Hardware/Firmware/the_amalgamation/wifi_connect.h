#include <WiFi.h>
void onConnect(WiFiEvent_t event, WiFiEventInfo_t info);
void onDisconnect(WiFiEvent_t event, WiFiEventInfo_t info);
//void onDisconnect(const WiFiEventStationModeDisconnected& event);
//void onConnect(const WiFiEventStationModeConnected& event);
void wifiConnect();
void wifi_init();
bool getWifiStatus();
