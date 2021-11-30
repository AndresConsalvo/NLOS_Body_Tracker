#include <Wire.h>
#include <WiFi.h>
#include <WiFiUdp.h>

#define BMX160_ADDRESS (0x68)

const char * networkName = "Jaxson_iPhone";
const char * networkPswd = "lizard02";

const char * udpAddress = "172.20.10.10";
const int udpPort = 20001;

boolean connected = false;

WiFiUDP udp;

// (Gx_L, Gx_H, Gy_L, Gy_H, Gz_L, Gz_H, Ax_L, Ax_H, Ay_L, Ay_H, Az_L, Az_H)
byte data[12];

short read_from_imu(byte reg_addr, byte num_bytes);

void write_to_imu(byte reg_addr, byte command);

void print_from_imu(void);

void setup() {
  Wire.begin(); // join I2C bus (no address = master)
  Wire.setClock(100000);
  write_to_imu(0x7E, (0x11 | 0x15));
  Serial.begin(9600);
  connectToWiFi(networkName, networkPswd);
}

void loop() {
  short temp;
  for (byte i = 0; i < 6; i++) {
    temp = read_from_imu((0x0C + 2 * i), 2);
    data[2 * i] = (byte) ((temp >> 0) & 0xFF); // lower byte
    data[2 * i + 1] = (byte) ((temp >> 8) & 0xFF); // upper byte
  }

  if (connected) {
    //Send a packet
    Serial.print("Still connected! \n");
    udp.beginPacket(udpAddress, udpPort);
    udp.write(data, 12);
    udp.endPacket();
  }
  //Wait for 1 second
  //delay(1000);
}

short read_from_imu(byte reg_addr, byte num_bytes) {
  short res_value = 0;
  Wire.beginTransmission((byte) BMX160_ADDRESS);
  Wire.write((uint8_t) reg_addr);
  if (Wire.endTransmission(false) != 0) return 0;
  Wire.requestFrom((byte) BMX160_ADDRESS, num_bytes);
  byte x = 0;
  int i = 0;
  while (Wire.available()) {
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

void print_from_imu() {
  
  for (byte i = 0; i < 6; i++){
      short temp = ((((short) data[2 * i+1]) << 8) | data[2 * i]);
      if (i < 5) {
          Serial.print(temp);
          Serial.print(",");
      }
      else {
          Serial.println(temp);
      }
    }
}

void connectToWiFi(const char * ssid, const char * pwd) {
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
void WiFiEvent(WiFiEvent_t event) {
  switch (event) {
    case SYSTEM_EVENT_STA_GOT_IP:
      //When connected set
      Serial.print("WiFi connected! IP address: ");
      Serial.println(WiFi.localIP());
      //initializes the UDP state
      //This initializes the transfer buffer
      udp.begin(WiFi.localIP(), udpPort);
      connected = true;
      break;
    case SYSTEM_EVENT_STA_DISCONNECTED:
      Serial.println("WiFi lost connection");
      connected = false;
      break;
  }
}
