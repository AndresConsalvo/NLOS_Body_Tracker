#include <Arduino.h>
#include "EEPROM.h"
#include "wifi_connect.h"
#include "io.h"
#include "udp.h"
#include "imu.h"

int serialLength;
char ssid[128];
char password[128];
char ip[5];
char port[16];

byte ip_arr[4];
short net_port;

int send_length;

int receive_length_ssid;
int receive_length_pass;
int receive_length_ip;
int receive_length_port;

void setup() {
  Serial.begin(115200);

  if (!EEPROM.begin(EEPROM_SIZE)) {
      Serial.println("failed to init EEPROM");
      delay(1000000);
  }
  
  ip[0] = EEPROM.read(0);
  ip[1] = EEPROM.read(1);
  ip[2] = EEPROM.read(2);
  ip[3] = EEPROM.read(3);
  net_port = EEPROM.read(4) << 8 | EEPROM.read(5);
    
  io_init();
  wifiConnect();
  imu_init();
}

void loop() {

  serialLength = Serial.available();
  
  if (serialLength) {
    receive_length_ssid = Serial.readBytesUntil('\n', ssid, sizeof(ssid));
    receive_length_pass = Serial.readBytesUntil('\n', password, sizeof(password));
    receive_length_ip   = Serial.readBytesUntil('\n', ip, sizeof(ip));
    receive_length_port = Serial.readBytesUntil('\n', port, sizeof(port));

    //wifiConnect(ssid, password);
    
    ssid[receive_length_ssid] = '\n';
    password[receive_length_pass] = '\n';
    
    send_length = Serial.write(ssid);
    Serial.write(password);
    
    memset(ssid, 0, sizeof(ssid));
    memset(password, 0, sizeof(password));
    memset(ip, 0, sizeof(ip));
  }
  
  printImuValues();
  data_send();  
  delay(10);
}
