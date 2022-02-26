#include <Arduino.h>
#include "wifi_connect.h"
#include "io.h"
#include "udp.h"
//#include "spi.h"

void setup() {
  Serial.begin(115200);
  io_init();
  wifiConnect();
//  spi_init();
}

void loop() {
  // put your main code here, to run repeatedly:
//  Serial.println("LEts go");
  test_send();
  //signalSOS();
//  spi_read(0x40);
  delay(100);

}
