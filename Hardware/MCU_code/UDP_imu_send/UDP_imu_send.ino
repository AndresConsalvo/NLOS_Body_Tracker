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

// (Gx, Gy, Gz, Ax, Ay, Az)
short short_data[6];
byte data[12];

short read_from_imu(byte reg_addr, byte num_bytes);

void write_to_imu(byte reg_addr, byte command);

void print_from_imu(void);

void setup() {
  Wire.begin(); // join I2C bus (no address = master)
  //set PMU mode of gyroscope to normal
  write_to_imu(0x7E, (0x15));
  //write 0b0100 to register 0x69
  //enables fast offset compensation for all three axis of gyro
  write_to_imu(0x69, (0x04));

  Wire.setClock(100000);
  Serial.begin(115200);
  connectToWiFi(networkName, networkPswd);

  // Green
  pinMode(D0, OUTPUT);
  // Red
  pinMode(D2, OUTPUT);
}

byte comm;
void loop() {
  for (byte i = 0; i < 6; i++) {
    short_data[i] = read_from_imu(0x0C + 2 * i);
    data[2 * i] = (short_data[i] >> 8) & 0xFF;
    data[2 * i + 1] = (short_data[i]) & 0xFF;
  }
  
  if (connected) {
    digitalWrite(D0, HIGH);
    digitalWrite(D2, LOW);
    //Send a packet
    Serial.print("Still connected! \n");
    udp.beginPacket(udpAddress, udpPort);
    udp.write(data, 12);
    udp.endPacket();
  } else {
    digitalWrite(D0, LOW);
    digitalWrite(D2, HIGH);
    Serial.print("Not connected\n");
  }
  delay(100);

//print_from_imu();
}

short read_from_imu(byte reg_addr) {
  Wire.beginTransmission((byte) BMX160_ADDRESS);
  Wire.write((uint8_t) reg_addr);
  if (Wire.endTransmission(false) != 0) return 0;
  Wire.requestFrom((byte) BMX160_ADDRESS, (byte) 2);
  
  short res_value = 0;
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
  Serial.print((short) short_data[0], HEX);
  Serial.print(",");
  Serial.print((short) short_data[1], HEX);
  Serial.print(",");
  Serial.print((short) short_data[2], HEX);
  Serial.print(",");
  Serial.print((short) short_data[3], HEX);
  Serial.print(",");
  Serial.print((short) short_data[4], HEX);
  Serial.print(",");
  Serial.println((short) short_data[5], HEX);
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
