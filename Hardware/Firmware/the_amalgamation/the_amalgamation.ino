#include <Arduino.h>
#include "wifi_connect.h"
#include "io.h"
#include "udp.h"
#include "imu.h"


void setup() {
  Serial.begin(115200);
  io_init();
  wifiConnect();
  imu_init();
}

void loop() {
  data_send();  
  delay(10);
}
