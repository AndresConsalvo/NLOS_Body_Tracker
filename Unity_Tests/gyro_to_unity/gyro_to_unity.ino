#include <Wire.h>
#define BMX160_ADDRESS (0x68)

// (Gx, Gy, Gz, Ax, Ay, Az)
short short_data[6];

short read_from_imu(byte reg_addr, byte num_bytes);

void write_to_imu(byte reg_addr, byte command);

void print_from_imu(void);

void setup() {
  Wire.begin(); // join I2C bus (no address = master)
  write_to_imu(0x7E, (0x15));

  Wire.setClock(100000);
  Serial.begin(9600);
}

byte comm;
void loop() {
  for (byte i = 0; i < 6; i++) {
    short_data[i] = read_from_imu(0x0C + 2 * i);
  }

  if (Serial.available()) {
    data = Serial.read();
    if (data == 'A') {
      Serial.println(short_data[3]);
    }
  }
  
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
    //res_value = (short) Wire.read() | (Wire.read() << 8);
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
  Serial.print((short) short_data[0]);
  Serial.print(",");
  Serial.print((short) short_data[1]);
  Serial.print(",");
  Serial.print((short) short_data[2]);
  Serial.print(",");
  Serial.print((short) short_data[3]);
  Serial.print(",");
  Serial.print((short) short_data[4]);
  Serial.print(",");
  Serial.println((short) short_data[5]);
}
