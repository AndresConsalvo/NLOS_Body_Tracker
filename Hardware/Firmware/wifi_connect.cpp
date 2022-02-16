//foundational code for establihing wifi connection and begining data transfer 
#include "wifi_connect.h"
#include "global_includes.h"
#define WIFI_NETWORK "my network name goes here"
#define WIFI_PASSWORD "password here"

//if cannot connect to wifi in x ms, abort
#define WIFI_TIMEOUT_MS 10000

void wifiConnect(){
    //disconnect
    WiFi.disconnect(true);
    //pick station mode to connect to existing network
    //idk if we need this 
    //WiFi.mode(WIFI_STA);
    //event handler
    WiFi.onEvent(WiFiEvent);
    //begin wifi wifi connection
    WiFi.begin(WIFI_NETWORK, WIFI_PASSWORD);


    //old code
    // unsigned long startConnectTime = millis();
    // //confirm connection status
    // while(WiFi.status () != WL_CONNECTED && millis () - startConnectTime < WIFI_TIMEOUT_MS)
    //     delay(100); 

    // //check connection
    // if(Wifi.status() == WL_CONNECTED){
    //     //connected
    // }
    // else{
    //     //not connected, SOS led
    // }
}



//attempt with return int
int WiFiEvent(WiFiEvent_t event){
    switch(event) {
      case ARDUINO_EVENT_WIFI_STA_GOT_IP:
          //When connected set 
          Serial.print("WiFi connected! IP address: ");
          Serial.println(WiFi.localIP());  
          //initializes the UDP state
          //This initializes the transfer buffer
          udp.begin(WiFi.localIP(),udpPort);
          return 1;
      case ARDUINO_EVENT_WIFI_STA_DISCONNECTED:
          Serial.println("WiFi lost connection");

          return 0;
      default: break;
    }
}

