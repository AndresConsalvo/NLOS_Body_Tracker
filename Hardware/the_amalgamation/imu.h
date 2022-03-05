struct sensor_data{
  float accel_x;
  float accel_y;
  float accel_z;

  float gyro_x;
  float gyro_y;
  float gyro_z;
};

sensor_data getMpuValues();
void imu_init();
void printImuValues();
