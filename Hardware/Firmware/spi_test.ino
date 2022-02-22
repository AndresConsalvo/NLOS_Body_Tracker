
/*
    SPI Master Demo Sketch
    Connect the SPI Master device to the following pins on the esp8266:

    GPIO    NodeMCU   Name  |   Uno
   ===================================
     15       D8       SS   |   D10
     13       D7      MOSI  |   D11
     12       D6      MISO  |   D12
     14       D5      SCK   |   D13

    Note: If the ESP is booting at a moment when the SPI Master has the Select line HIGH (deselected)
    the ESP8266 WILL FAIL to boot!
    See SPISlave_SafeMaster example for possible workaround

*/
#include <Arduino.h>
#include <SPI.h>
  const int CS_Pin = 15;

void setup() {
  // put your setup code here, to run once:
  pinMode(15, OUTPUT);
  pinMode(13, OUTPUT);
  pinMode(12, INPUT);
  pinMode(14, OUTPUT);
  Serial.begin(115200);
  SPCR |= bit(SPE);
  SPI.begin();
}

void loop() {
  //delay(1000);
  Serial.print("Transaction");
  SPI.beginTransaction(SPISettings(500, MSBFIRST, SPI_MODE3));
  digitalWrite(15, LOW);
  int x = SPI.transfer(0x26); //Send register location
  digitalWrite(CS_Pin, HIGH);
  SPI.endTransaction();
}
