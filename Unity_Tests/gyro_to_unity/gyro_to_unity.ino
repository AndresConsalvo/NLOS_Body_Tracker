#include <Wire.h>
#define BMX160_ADDRESS (0x68)

// (Gx_L, Gx_H, Gy_L, Gy_H, Gz_L, Gz_H, Ax_L, Ax_H, Ay_L, Ay_H, Az_L, Az_H)
byte data[12];

short read_from_imu(byte reg_addr, byte num_bytes);

void write_to_imu(byte reg_addr, byte command);

void print_from_imu(void);

void setup() {
  Wire.begin(); // join I2C bus (no address = master)
  write_to_imu(0x7E, (0x11 | 0x15));
  Wire.setClock(100000);
  Serial.begin(9600);
}

void loop() {
  // When Unity needs the gyroscope data, it sends a character
  // Store gyroscope data as a packet, send it all at once
  // 6 bytes
  short temp;
  for (byte i = 0; i < 6; i++) {
    temp = read_from_imu((0x0C + 2 * i), 2);
    data[2 * i] = (byte) ((temp >> 0) & 0xFF); // lower byte
    data[2 * i + 1] = (byte) ((temp >> 8) & 0xFF); // upper byte
  }

  Serial.print((short) ((short) data[1] << 8) | data[0]);
  Serial.print(",");
  Serial.print((short) ((short) data[3] << 8) | data[2]);
  Serial.print(",");
  Serial.print((short) ((short) data[5] << 8) | data[4]);
  Serial.print(",");
  Serial.print((short) ((short) data[7] << 8) | data[6]);
  Serial.print(",");
  Serial.print((short) ((short) data[9] << 8) | data[8]);
  Serial.print(",");
  Serial.print((short) ((short) data[11] << 8) | data[10]);
  Serial.println(",");
  
  
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
  short temp;
  for (byte i = 0; i < 6; i++){
      temp = ((((short) data[2 * i+1]) << 8) | data[2 * i]);
      if (i < 5) {
          Serial.print(temp);
          Serial.print(",");
      }
      else {
          Serial.println(temp);
      }
    }
}
