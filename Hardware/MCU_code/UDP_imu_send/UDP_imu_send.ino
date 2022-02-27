#include <Wire.h>
#include <WiFi.h>
#include <WiFiUdp.h>

#define BMX160_ADDRESS (0x68)

const char * networkName = "uam8nsw9dt9q";
const char * networkPswd = "K-jRAw9YC5-U";

const char * udpAddress = "192.168.1.59";
const int udpPort = 20000;

boolean connected = false;

WiFiUDP udp;

// (Gx, Gy, Gz, Ax, Ay, Az)
short short_data[6];
byte data[13];

short read_from_imu(byte reg_addr, byte num_bytes);

void write_to_imu(byte reg_addr, byte command);

void print_from_imu(void);

void setup() {
  Wire.begin(); // join I2C bus (no address = master)
  write_to_imu(0x7E, (0x15));
  write_to_imu(0x69, (0x04));
  write_to_imu(0x43, (0b00000000));

  Wire.setClock(100000);
  Serial.begin(115200);
  connectToWiFi(networkName, networkPswd);

  // Green
  pinMode(D0, OUTPUT);
  // Red
  pinMode(D2, OUTPUT);
}

unsigned short count;
unsigned short resetting = 0;
byte comm;
void loop() {
  for (byte i = 0; i < 6; i++) {
    short_data[i] = read_from_imu(0x0C + 2 * i);
    data[2 * i] = (short_data[i] >> 8) & 0xFF;
    data[2 * i + 1] = (short_data[i]) & 0xFF;
  }

  data[12] = count++ % 4;
  
  if (connected) {
    if (resetting < 500) {
      resetting++;
      data[12] = 4;
    }
    digitalWrite(D0, HIGH);
    digitalWrite(D2, LOW);
    //Send a packet
    Serial.print("Still connected! \n");
    udp.beginPacket(udpAddress, udpPort);
    udp.write(data, 13);
    udp.endPacket();
  } else {
    digitalWrite(D0, LOW);
    digitalWrite(D2, HIGH);
    Serial.print("Not connected\n");
  }
  delay(10);

 print_from_imu();
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
  Serial.print((short) short_data[0], DEC);
  Serial.print(",");
  Serial.print((short) short_data[1], DEC);
  Serial.print(",");
  Serial.print((short) short_data[2], DEC);
  Serial.print(",");
  Serial.print((short) short_data[3], DEC);
  Serial.print(",");
  Serial.print((short) short_data[4], DEC);
  Serial.print(",");
  Serial.print((short) short_data[5], DEC);
  Serial.print(",");
  Serial.println((short) data[12], DEC);
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
