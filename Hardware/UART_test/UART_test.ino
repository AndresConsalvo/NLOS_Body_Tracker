#include <WiFi.h>

int serialLength;
char r[5];
char ssid[128];
char password[128];
char ip[5];
char port[16];
char trackerID_recv[5];

int send_length;
int receive_length_ssid;
int receive_length_pass;
int receive_length_ip;
int receive_length_port;
int receive_length_trackerID;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  Serial.println("Hello, world!");

  //WiFi.onEvent(WiFiEvent);
  //WiFi.begin();
}

void loop() {
  // put your main code here, to run repeatedly:
  serialLength = Serial.available();
  
  if (serialLength) {
    receive_length_ssid = Serial.readBytesUntil('\n', ssid, sizeof(ssid));
    receive_length_pass = Serial.readBytesUntil('\n', password, sizeof(password));

    ssid[receive_length_ssid] = '\n';
    password[receive_length_pass] = '\n';

    
    send_length = Serial.write(ssid);
    Serial.write(password);
    
    memset(ssid, 0, sizeof(ssid));
    memset(password, 0, sizeof(password));
  }
}

void wifiConnect(const char* ssid, const char* pass) {
  delay(5000);
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
      //initializes the UDP state
      //This initializes the transfer buffer
      break;
    case SYSTEM_EVENT_STA_DISCONNECTED:
      Serial.println("WiFi lost connection");
      break;
  }
}
