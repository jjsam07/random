/*
 * SS – digital 10. You can use other digital pins, but 10 is generally the default as it is next to the other SPI pins;
 * MOSI – digital 11;
 * MISO – digital 12;
 * SCK – digital 13;
 */

#include <SPI.h>

void setup() {
  Serial.begin(115200);
  SPI.begin();
  pinMode(10, OUTPUT);
}

unsigned char upper = 0;
unsigned char middle = 0;
unsigned char lower = 0;
unsigned char tempbuffer = 0;
unsigned char input[5] = {};

void loop() {
  if (Serial.available() > 0) {
    Serial.readBytes(input, 5);
    upper = input[2];
    middle = input[3];
    lower = input[4];
    for (int i = 0; i <= input[1]; i++) {
      digitalWrite(10, LOW);
      SPI.transfer(3);
      SPI.transfer(upper);
      SPI.transfer(middle);
      SPI.transfer(lower);
      tempbuffer = SPI.transfer(0);
      digitalWrite(10, HIGH);
      if (lower != 0xff) {
        lower++;
      } else {
        lower = 0;
        if (middle != 0xff) {
          middle++;
        } else {
          middle = 0;
          if (upper != 0xff) {
            upper++;
          } else {
            break;
          }
        }
      }
      Serial.write(tempbuffer);
    }
    tempbuffer = 0;

  }
}
