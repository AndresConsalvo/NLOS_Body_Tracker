#include <Arduino.h>
#include <WiFi.h>
#include <WiFiUdp.h>
#include <stdio.h>
#include "defines.h"
#include "EEPROM.h"
#include "I2Cdev.h"
#include "MPU6050_6Axis_MotionApps20.h"
#include "Wire.h"


#define TIMER_INTERVAL_MS       250
#define EEPROM_SIZE 64

MPU6050 mpu;

WiFiUDP Udp;

int STATUS_LED_PIN = 2;

byte led_cycle[27] = {HIGH, LOW, HIGH, LOW, HIGH, LOW, LOW, LOW, HIGH, HIGH, LOW, HIGH, HIGH, LOW, HIGH, HIGH, LOW, LOW, LOW, HIGH, LOW, HIGH, LOW, HIGH, LOW, LOW, LOW};
byte state = LOW;
int count = 0;
hw_timer_t * timer = NULL;

int serialLength;
char ssid[128];
char password[128];
char ip[5];
char port[16];
char trackerID_recv[2];

int TrackerID = 0;
byte ip_arr[4];
IPAddress udpAddress(0, 0, 0, 0);
uint16_t net_port;

int send_length;

int receive_length_ssid;
int receive_length_pass;
int receive_length_ip;
int receive_length_port;
int receive_length_trackerID;

struct sensor_data{
  float accel_x;
  float accel_y;
  float accel_z;

  float gyro_x;
  float gyro_y;
  float gyro_z;

  float quat_w;
  float quat_x;
  float quat_y;
  float quat_z;
  
};

// MPU control/status vars
bool dmpReady = false;  // set true if DMP init was successful
uint8_t mpuIntStatus;   // holds actual interrupt status byte from MPU
uint8_t devStatus;      // return status after each device operation (0 = success, !0 = error)
uint16_t packetSize;    // expected DMP packet size (default is 42 bytes)
uint16_t fifoCount;     // count of all bytes currently in FIFO
uint8_t fifoBuffer[64]; // FIFO storage buffer

// orientation/motion vars
Quaternion q;           // [w, x, y, z]         quaternion container
VectorInt16 aa;         // [x, y, z]            accel sensor measurements
VectorInt16 aaReal;     // [x, y, z]            gravity-free accel sensor measurements
VectorFloat gravity;    // [x, y, z]            gravity vector
int16_t gyro[3];

/**********************************************FUNCTION PROTOTYPES*************************************************/
void imu_init();
void printImuValues();
sensor_data getMpuValues();

void io_init();
void TimerHandler();

void udp_init();
void test_send();
void data_send();

void wifiConnect();
void wifi_init();
bool getWifiStatus();


/***********************************************ARDUINO MAIN BODY**************************************************/
void setup() {

  Serial.begin(115200);

  Wire.begin();
  Wire.setClock(400000);

  if (!EEPROM.begin(EEPROM_SIZE)) {
      Serial.println("failed to init EEPROM");
      delay(1000000);
  }
  
  ip[0] = EEPROM.read(0);
  ip[1] = EEPROM.read(1);
  ip[2] = EEPROM.read(2);
  ip[3] = EEPROM.read(3);


  
  io_init();
  WiFi.onEvent(WiFiEvent);
  WiFi.begin();

  udpAddress = IPAddress(ip[0], ip[1], ip[2], ip[3]);
  net_port = EEPROM.read(4) << 8 | EEPROM.read(5);
  TrackerID = EEPROM.read(6);
  
  imu_init();
}

void loop() {
  serialLength = Serial.available();
  
  if (serialLength) {
    receive_length_ssid = Serial.readBytesUntil('\n', ssid, sizeof(ssid));
    receive_length_pass = Serial.readBytesUntil('\n', password, sizeof(password));
    receive_length_ip   = Serial.readBytesUntil('\n', ip, sizeof(ip));
    receive_length_port = Serial.readBytesUntil('\n', port, sizeof(port));
    receive_length_trackerID = Serial.readBytesUntil('\n', trackerID_recv, sizeof(trackerID_recv)); 

    wifiConnect(ssid, password);
    
    ip_arr[0] = ip[0];
    ip_arr[1] = ip[1];
    ip_arr[2] = ip[2];
    ip_arr[3] = ip[3];
    
    udpAddress = IPAddress(ip[0], ip[1], ip[2], ip[3]);
    net_port = (port[0] << 8) | port[1];
    TrackerID = trackerID_recv[0];
    
    // writing byte-by-byte to EEPROM
    EEPROM.write(0, ip[0]);
    EEPROM.write(1, ip[1]);
    EEPROM.write(2, ip[2]);
    EEPROM.write(3, ip[3]);
    EEPROM.write(4, port[0]);
    EEPROM.write(5, port[1]);
    EEPROM.write(6, trackerID_recv[0]);

    EEPROM.commit();
    
    memset(ssid, 0, sizeof(ssid));
    memset(password, 0, sizeof(password));
    memset(ip, 0, sizeof(ip));
    memset(port, 0, sizeof(port));
    memset(trackerID_recv, 0, sizeof(trackerID_recv));
  }
  
  data_send();
  //printImuValues();  
  delay(10);
}
/********************************************END OF ARDUINO MAIN BODY*********************************************/

/********************************************** FUNCTION DEFINES**************************************************/

void imu_init(){

  devStatus = mpu.dmpInitialize();

  //mpu.setXGyroOffset(-1914);
  //mpu.setYGyroOffset(-267);
  //mpu.setZGyroOffset(1046);
  //mpu.setXAccelOffset(1760); // 1688 factory default for my test chip
  //mpu.setYAccelOffset(1760); // 1688 factory default for my test chip
  //mpu.setZAccelOffset(1760); // 1688 factory default for my test chip
  
  if (devStatus) {
    Serial.println("Failed to find MPU6050 chip");
    while (1) {
      delay(10);
    }
  }

  mpu.CalibrateAccel(6);
  mpu.CalibrateGyro(6);
  mpu.PrintActiveOffsets();
  mpu.setDMPEnabled(true);

  dmpReady = true;
  packetSize = mpu.dmpGetFIFOPacketSize();
}

sensor_data getMpuValues(){
  if (mpu.dmpGetCurrentFIFOPacket(fifoBuffer)) {
      mpu.dmpGetGyro(gyro, fifoBuffer);

      mpu.dmpGetQuaternion(&q, fifoBuffer);
      mpu.dmpGetAccel(&aa, fifoBuffer);
      mpu.dmpGetGravity(&gravity, &q);
      mpu.dmpGetLinearAccel(&aaReal, &aa, &gravity);
  }

  sensor_data s;
  s.accel_x = aaReal.x;
  s.accel_y = aaReal.y;
  s.accel_z = aaReal.z;

  if (abs(gyro[0]) < 5) {
    s.gyro_x = 0;
  } else {
    s.gyro_x = ((float)gyro[1]) * M_PI/180.0 * 0.05;
  }

  if (abs(gyro[1]) < 5) {
    s.gyro_y = 0;
  } else {
    s.gyro_y = -((float)gyro[0]) * M_PI/180.0 * 0.05;
  }

  if (abs(gyro[2]) < 5) {
    s.gyro_z = 0;
  } else {
    s.gyro_z = ((float)gyro[2]) * M_PI/180.0 * 0.05;
  }

  s.quat_w = q.w;
  s.quat_x = q.x;
  s.quat_y = q.y;
  s.quat_z = q.z;
  
  return s;
}

void printImuValues(){
  sensor_data s = getMpuValues();
  //Serial.print("Acceleration X: ");
  //Serial.print(s.accel_x);
 // Serial.print(", Y: ");
  //Serial.print(s.accel_y);
  //Serial.print(", Z: ");
  //Serial.print(s.accel_z);
  //Serial.println(" m/s^2");
  Serial.print(gyro[0]);
  Serial.print(gyro[1]);
  Serial.println(gyro[2]);

  //Serial.print("Rotation X: ");
  Serial.print(s.gyro_x);
  Serial.print(" ");
  //Serial.println(", Y: ");
  Serial.print(s.gyro_y);
  Serial.print(" ");
  //Serial.print(", Z: ");
  Serial.println(s.gyro_z);

  //Serial.println(" rad/s");

  Serial.print("quat\t");
  Serial.print(q.w);
  Serial.print("\t");
  Serial.print(q.x);
  Serial.print("\t");
  Serial.print(q.y);
  Serial.print("\t");
  Serial.println(q.z);
}

void TimerHandler(){ 
  if(getWifiStatus()){
    count += 1;
    if(count >= 3){
      state = !state;
      count = 0;
    }
    digitalWrite(STATUS_LED_PIN, state);
  }
  else{
    digitalWrite(STATUS_LED_PIN, led_cycle[count]);
    count = (count >= 26) ? 0 : count + 1;
  }
}

void timer_init(){
  timer = timerBegin(0, 80, true);
  timerAttachInterrupt(timer, &TimerHandler, true);
  timerAlarmWrite(timer, 1000 * TIMER_INTERVAL_MS, true);
  timerAlarmEnable(timer);
}

void io_init(){
    pinMode(STATUS_LED_PIN, OUTPUT);
    pinMode(0, OUTPUT);
    digitalWrite(0, HIGH);
    timer_init();
}

/*
   *{
   *"type" : "POSITION",
   *"data" : {
   *"accel" : [x,y,z],
   *"gyro" : [x,y,z],
   *"quat" : [w,x,y,z]
   *}
   */
int packet_count = 100;
void data_send(){  
  sensor_data s = getMpuValues();
  char ReplyBuffer[256];
  
  char  textBuffer[] = "{\"type\": \"POSITION\", \"data\": {\"accel\": [";
  char commaBuffer[] = ",";
  sprintf(ReplyBuffer, "%s%.2f%s%.2f%s%.2f%s%.4f%s%.4f%s%.4f%s%.4f%s%.4f%s%.4f%s%.4f%s%s%s", textBuffer, s.accel_x, ",", s.accel_y, ",", s.accel_z, "], \"gyro\": [", s.gyro_x, ",", s.gyro_y, ",", s.gyro_z, "], \"quat\": [", s.quat_w,",",s.quat_x,",",s.quat_y,",",s.quat_z,"], \"id\": ", String(TrackerID), "}}");
  
  if (udpAddress != IPAddress(0, 0, 0, 0)) {

   
      Serial.print(udpAddress);
      Serial.println(net_port);
      Udp.beginPacket(udpAddress, net_port);
      Udp.printf(ReplyBuffer);
      Udp.endPacket();

      if(packet_count == 200){
        Serial.println(" 200 Packets sent");
        Serial.println(ReplyBuffer);
        packet_count = 0;
      }
      packet_count += 1;
  }
}

void wifiConnect(const char* ssid, const char* pass) {
  Serial.println("Connecting to WiFi network: " + String(ssid));

  // delete old config
  WiFi.disconnect(true);
  //register event handler
  WiFi.onEvent(WiFiEvent);

  //Initiate connection
  WiFi.begin(ssid, pass);

  Serial.println("Waiting for WIFI connection...");
}

void WiFiEvent(WiFiEvent_t event) {
  switch (event) {
    case SYSTEM_EVENT_STA_GOT_IP:
      //When connected set
      Serial.print("WiFi connected! IP address: ");
      Serial.println(WiFi.localIP());
      Serial.print("SSID: ");
      Serial.println(WiFi.SSID());

      Udp.begin(WiFi.localIP(), 4242);
      //initializes the UDP state
      //This initializes the transfer buffer
      break;
    case SYSTEM_EVENT_STA_DISCONNECTED:
      Serial.println("WiFi lost connection");
      break;
  }
}

bool getWifiStatus(){
  return (WiFi.status() == WL_CONNECTED) ? 1 : 0;
}
