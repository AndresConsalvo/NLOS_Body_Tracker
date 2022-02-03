//foundational code for establihing wifi connection and begining data transfer 

#include <Arduino.h>
#include "WiFi.h"

#define WIFI_NETWORK "my network name goes here"
#define WIFI_PASSWORD "password here"
//if cannot connect to wifi in x ms, abort
#define WIFI_TIMEOUT_MS 10000

void wifiConnect(){
    //pick station mode to connect to existing network
    WiFi.mode(WIFI_STA);

    //begin wifi wifi connection
    WiFi.begin(WIFI_NETWORK, WIFI_PASSWORD);

    unsigned long startConnectTime = millis();
    //confirm connection status
    while(WiFi.status () != WL_CONNECTED && millis () - startConnectTime < WIFI_TIMEOUT_MS)
        delay(100); 

    //check connection
    if(Wifi.status() == WL_CONNECTED){
        //connected
    }
    else{
        //not connected
    }
}
