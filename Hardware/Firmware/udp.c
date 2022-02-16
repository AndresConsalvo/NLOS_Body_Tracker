#include <WiFi.h>
#include <WiFiUdp.h>
const char * udpAddress = "192.168.0.255";
const int udpPort = 3333;
WiFiUDP udp;
void udpTransmit(){
    
    //TODO figure out the size of data thats transmitted
    uint8_t buffer[100] = "udp transmission :)))))";
    udp.beginPacket
    udp.beginPacket(udpAddress, udpPort);
    udp.write(buffer, 11);
    udp.endPacket();
    memset(buffer, 0, 50);
    //processing incoming packet, must be called before reading the buffer
    udp.parsePacket();
    //receive response from server, it will be HELLO WORLD
    if(udp.read(buffer, 50) > 0){
        Serial.print("Server to client: ");
        Serial.println((char *)buffer);
    }
    //Wait for 1 second
    delay(1000);
}