#include <Arduino.h>
#include "io.h"
//
//void set
//Serial.println("Beginning...")
//
//wifiConnect();

void setup() {
  io_init();
  
}

void loop() {
  // put your main code here, to run repeatedly:
  signalSOS();
}
