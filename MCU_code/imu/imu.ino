#include <Wire.h>
#include <WiFi.h>
#include <WiFiUdp.h>

const char * networkName = "uam8nsw9dt9q";
const char * networkPswd = "K-jRAw9YC5-U";

const char * udpAddress = "192.168.1.31";
const int udpPort = 20001;

boolean connected = false;

WiFiUDP udp;

// (Ax_L, Ax_H, Ay_L, Ay_H, Az_L, Az_H, Gx_L, Gx_H, Gy_L, Gy_H, Gz_L, Gz_H)
byte data[12];


#define BMX160_ADDRESS (0x68)
#define A_X_addr  (0x12)
void setup() {
  // put your setup code here, to run once:
  Wire.begin(); // join I2C bus (no address = master)
  Wire.setClock(100000);
  Serial.begin(9600);
  connectToWiFi(networkName, networkPswd);
}

short read_from_imu(byte reg_addr, byte num_bytes);

void write_to_imu(byte reg_addr, byte command);

void loop() { 
  write_to_imu(0x7E, (0x11));

  short accel_x, accel_y, accel_z;
  short gyro_x, gyro_y, gyro_z;

  accel_x = read_from_imu((byte) 0x12, 2);
  accel_y = read_from_imu((byte) 0x14, 2);
  accel_z = read_from_imu((byte) 0x16, 2);
  
  gyro_x = read_from_imu((byte) 0x0C, 2);
  gyro_y = read_from_imu((byte) 0x0E, 2);
  gyro_z = read_from_imu((byte) 0x10, 2);
  
  data[0] = (byte) ((accel_x >> 0) & 0xFF);
  data[1] = (byte) ((accel_x >> 8) & 0xFF);
  data[2] = (byte) ((accel_y >> 0) & 0xFF);
  data[3] = (byte) ((accel_y >> 8) & 0xFF);
  data[4] = (byte) ((accel_z >> 0) & 0xFF);
  data[5] = (byte) ((accel_z >> 8) & 0xFF);
  data[6] = (byte) ((gyro_x >> 0) & 0xFF);
  data[7] = (byte) ((gyro_x >> 8) & 0xFF);
  data[8] = (byte) ((gyro_y >> 0) & 0xFF);
  data[9] = (byte) ((gyro_y >> 8) & 0xFF);
  data[10] = (byte) ((gyro_z >> 0) & 0xFF);
  data[11] = (byte) ((gyro_z >> 8) & 0xFF);

  

//  data[0] = accel_x;
//  data[2] = accel_y;
//  data[4] = accel_z;
//  data[6] = gyro_x;
//  data[8] = gyro_y;
//  data[10] = gyro_z;

  if(connected){
    //Send a packet
    Serial.print("Still connected! \n");
    udp.beginPacket(udpAddress,udpPort);
    udp.write(data, 12);
    udp.endPacket();
  }
  //Wait for 1 second
  delay(1000);

  Serial.print(accel_x, HEX);
  Serial.print(",");
  Serial.print(accel_y, HEX);
  Serial.print(",");
  Serial.print(accel_z, HEX);
  Serial.print(",");
  Serial.print(gyro_x, HEX);
  Serial.print(",");
  Serial.print(gyro_y, HEX);
  Serial.print(",");
  Serial.println(gyro_z, HEX);

}

short read_from_imu(byte reg_addr, byte num_bytes) {
  short res_value = 0;

  Wire.beginTransmission((byte) BMX160_ADDRESS);
  Wire.write((uint8_t) reg_addr);
  if (Wire.endTransmission(false) != 0) return 0;
  Wire.requestFrom((byte) BMX160_ADDRESS, num_bytes);
  byte x = 0;
  int i = 0;
  
  while(Wire.available()) {
    x = Wire.read();
    res_value += x << (8 * i++);
  }
  
  return res_value;
}


void write_to_imu(byte reg_addr, byte command) {
  Wire.beginTransmission((byte) BMX160_ADDRESS);
  Wire.write((uint8_t) reg_addr);
  Wire.write((uint8_t) command);
  if (Wire.endTransmission(false) != 0) return;
}

void connectToWiFi(const char * ssid, const char * pwd){
  Serial.println("Connecting to WiFi network: " + String(ssid));

  // delete old config
  WiFi.disconnect(true);
  //register event handler
  WiFi.onEvent(WiFiEvent);

  //Initiate connection
  WiFi.begin(ssid, pwd);

  Serial.println("Waiting for WIFI connection...");
}

//wifi event handler
void WiFiEvent(WiFiEvent_t event){
    switch(event) {
      case SYSTEM_EVENT_STA_GOT_IP:
          //When connected set 
          Serial.print("WiFi connected! IP address: ");
          Serial.println(WiFi.localIP());  
          //initializes the UDP state
          //This initializes the transfer buffer
          udp.begin(WiFi.localIP(),udpPort);
          connected = true;
          break;
      case SYSTEM_EVENT_STA_DISCONNECTED:
          Serial.println("WiFi lost connection");
          connected = false;
          break;
    }
}
