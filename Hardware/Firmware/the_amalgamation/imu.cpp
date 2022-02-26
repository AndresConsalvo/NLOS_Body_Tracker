#include <Adafruit_MPU6050.h>
#include <Adafruit_Sensor.h>
#include <Wire.h>
#include "imu.h"

Adafruit_MPU6050 mpu;

void imu_init(){
  if (!mpu.begin()) {
    Serial.println("Failed to find MPU6050 chip");
    while (1) {
      delay(10);
    }
  }
  mpu.setAccelerometerRange(MPU6050_RANGE_8_G);
  mpu.setGyroRange(MPU6050_RANGE_500_DEG);
  mpu.setFilterBandwidth(MPU6050_BAND_5_HZ);
}

sensor_data getMpuValues(){
  sensors_event_t a, g, temp;
  mpu.getEvent(&a, &g, &temp);
  sensor_data s;
  s.accel_x = a.acceleration.x;
  s.accel_y = a.acceleration.y;
  s.accel_z = a.acceleration.z;
  s.gyro_x = g.gyro.x;
  s.gyro_y = g.gyro.y;
  s.gyro_z = g.gyro.z;
  
  return s;
}

void printImuValues(){
  sensor_data s = getMpuValues();
  Serial.print("Acceleration X: ");
  Serial.print(s.accel_x);
  Serial.print(", Y: ");
  Serial.print(s.accel_y);
  Serial.print(", Z: ");
  Serial.print(s.accel_z);
  Serial.println(" m/s^2");

  Serial.print("Rotation X: ");
  Serial.print(s.gyro_x);
  Serial.print(", Y: ");
  Serial.print(s.gyro_y);
  Serial.print(", Z: ");
  Serial.print(s.gyro_z);
  Serial.println(" rad/s");
  
  Serial.println("");
}
