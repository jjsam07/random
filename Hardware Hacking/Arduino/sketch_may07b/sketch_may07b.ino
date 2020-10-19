#include <SPI.h>

void setup() {
  Serial.begin(115200);
  SPI.begin();
  pinMode(10, OUTPUT);
}

//unsigned char data0 = 0;
//unsigned char data1 = 0;
//unsigned char data2 = 0;
//unsigned char data3 = 0;
unsigned char upper = 0;
unsigned char middle = 0;
unsigned char lower = 0;
unsigned char tempbuffer = 0;
unsigned char input[5] = {};

void loop() {
  
  if (Serial.available() > 0) {
    
    Serial.readBytes(input, 5);
    
    if (input[0] == 0) {
      
      //Serial.println("Multiple read");
      digitalWrite(10, LOW);
      //data0 = SPI.transfer(3);
      //data1 = SPI.transfer(input[2]);
      //data2 = SPI.transfer(input[3]);
      //data3 = SPI.transfer(input[4]);
      SPI.transfer(3);
      SPI.transfer(input[2]);
      SPI.transfer(input[3]);
      SPI.transfer(input[4]);
      for (int i = 0; i < input[1]; i++) {
        tempbuffer = SPI.transfer(0);
        Serial.write(tempbuffer);
      }
      digitalWrite(10, HIGH);
      tempbuffer = 0;
      //Serial.println("DEBUG");
      //Serial.write(data0);
      //Serial.write(data1);
      //Serial.write(data2);
      //Serial.write(data3);
      
    } else if (input[0] == 1) {
      
      upper = input[2];
      middle = input[3];
      lower = input[4];
      //Serial.println("Single read");
      for (int i = 0; i < input[1]; i++) {
        digitalWrite(10, LOW);
        //data0 = SPI.transfer(3);
        //data1 = SPI.transfer(input[2]);
        //data2 = SPI.transfer(input[3]);
        //data3 = SPI.transfer(input[4]);
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
      //Serial.println("DEBUG");
      //Serial.write(data0);
      //Serial.write(data1);
      //Serial.write(data2);
      //Serial.write(data3);
      tempbuffer = 0;
      
    } else if (input[0] == 2) {
      
      while (true) {
        digitalWrite(10, LOW);
        //data0 = SPI.transfer(3);
        //data1 = SPI.transfer(input[2]);
        //data2 = SPI.transfer(input[3]);
        //data3 = SPI.transfer(input[4]);
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

    }  else {
      Serial.println("Invalid");
    }
  }
}
