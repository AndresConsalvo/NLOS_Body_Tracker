int data;

void setup() {
  Serial.begin(9600);
  pinMode(D9, OUTPUT);
  digitalWrite(D9, LOW);
}

void loop() {
  if (Serial.available()) {
    data = Serial.read();
    if (data == 'A') {
      digitalWrite(D9, HIGH);
    } else if (data == 'B') {
      digitalWrite(D9, LOW);
    }
  }
  delay(100);
}
