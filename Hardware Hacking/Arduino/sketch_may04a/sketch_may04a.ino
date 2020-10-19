void setup() {
  Serial.begin(500000);
}

void loop() {
  if (Serial.available() > 0) {
    Serial.print(Serial.readString());
  }
}
