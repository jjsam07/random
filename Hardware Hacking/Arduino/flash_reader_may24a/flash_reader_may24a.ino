/*
   SS – digital 10. You can use other digital pins, but 10 is generally the default as it is next to the other SPI pins;
   MOSI – digital 11;
   MISO – digital 12;
   SCK – digital 13;
*/

#include <SPI.h>

enum {
  WIP = 0x1,
  WEL = 0x2,
  BP0 = 0x4,
  BP1 = 0x8,
  BP2 = 0x10,
  BP3 = 0x20,
  SRWD = 0x80
};

SPISettings settingsA(1000000, MSBFIRST, SPI_MODE0);
SPISettings settingsB(1000000, LSBFIRST, SPI_MODE0);
SPISettings settingsC(1000000, MSBFIRST, SPI_MODE3);
SPISettings settingsD(1000000, LSBFIRST, SPI_MODE3);

void setup() {
  Serial.begin(1000000);
  SPI.begin();
  pinMode(10, OUTPUT);
}

unsigned long flashsize = 0x800000;
unsigned char input[7] = {};
unsigned char tempbuffer = 0;
unsigned char temp = 0;
unsigned char page_chunk0[64] = {};
unsigned char page_chunk1[64] = {};
unsigned char page_chunk2[64] = {};
unsigned char page_chunk3[64] = {};
unsigned char data = 0;
unsigned char upper = 0;
unsigned char middle = 0;
unsigned char lower = 0;
unsigned char statusregister = 0;
unsigned int counter = 0;

void serial_flush(void) {
  while (true)
  {
    delay (20);  // give data a chance to arrive
    if (Serial.available())
    {
      // we received something, get all of it and discard it
      while (Serial.available())
        Serial.read();
      continue;  // stay in the main loop
    }
    else
      break;  // nothing arrived for 20 ms
  }
}

void receive_input() {
  serial_flush(); // flush serial input buffer
  Serial.write(0xA5); // Ready signal
  while (true) {
    if (Serial.available()) {
      Serial.readBytes(page_chunk0, 64);
      break;
    }
  }
  if (input[5] > 64) {
    serial_flush(); // flush serial input buffer
    Serial.write(0xA5); // Ready signal
    while (true) {
      if (Serial.available()) {
        Serial.readBytes(page_chunk1, 64);
        break;
      }
    }
  }
  if (input[5] > 128) {
    serial_flush(); // flush serial input buffer
    Serial.write(0xA5); // Ready signal
    while (true) {
      if (Serial.available()) {
        Serial.readBytes(page_chunk2, 64);
        break;
      }
    }
  }
  if (input[5] > 192) {
    serial_flush(); // flush serial input buffer
    Serial.write(0xA5); // "Ready" signal
    while (true) {
      if (Serial.available()) {
        Serial.readBytes(page_chunk3, 64);
        break;
      }
    }
  }
}

void write_flash() {
  switch (input[6]) {
    case 0:
      SPI.beginTransaction(settingsA);
      break;
    case 1:
      SPI.beginTransaction(settingsB);
      break;
    case 2:
      SPI.beginTransaction(settingsC);
      break;
    case 3:
      SPI.beginTransaction(settingsD);
      break;
    default:
      break;
  }

  digitalWrite(10, LOW);
  SPI.transfer(0x06); // Write Enable command
  digitalWrite(10, HIGH);

  digitalWrite(10, LOW);
  SPI.transfer(0x05); // Read Status Register command
  statusregister = SPI.transfer(0); // Status register
  digitalWrite(10, HIGH);

  if ((statusregister & WEL) && !(statusregister & WIP)) {
    digitalWrite(10, LOW);
    SPI.transfer(0x02);
    SPI.transfer(input[1]);
    SPI.transfer(input[2]);
    SPI.transfer(input[3]);
    counter = 0;
    while (true) {
      if (counter == 64  || counter == input[5]) break;
      SPI.transfer(page_chunk0[counter]);
      counter++;
    }
    if (input[5] > 64) {
      temp = input[5] - 64;
      counter = 0;
      while (true) {
        if (counter == 64 || counter == temp) break;
        SPI.transfer(page_chunk1[counter]);
        counter++;
      }
    }
    if (input[5] > 128) {
      temp = input[5] - 128;
      counter = 0;
      while (true) {
        if (counter == 64 || counter == temp) break;
        SPI.transfer(page_chunk2[counter]);
        counter++;
      }
    }
    if (input[5] > 192) {
      temp = input[5] - 192;
      counter = 0;
      while (true) {
        if (counter == 64 || counter == temp) break;
        SPI.transfer(page_chunk3[counter]);
        counter++;
      }
    }
    digitalWrite(10, HIGH);
    SPI.endTransaction();
    Serial.write(0xDB); // "Done" signal
  } else {
    Serial.println("Cannot write to flash");
    SPI.endTransaction();
  }
}

void loop() {
  if (Serial.available()) {
    Serial.readBytes(input, 7);
    if (input[0] == 0) { //Mode 0: Continuous read. usage: bytearray([MODE, IGNORE, IGNORE, IGNORE, IGNORE, IGNORE, SPISETTING])
      switch (input[6]) {
        case 0:
          SPI.beginTransaction(settingsA);
          break;
        case 1:
          SPI.beginTransaction(settingsB);
          break;
        case 2:
          SPI.beginTransaction(settingsC);
          break;
        case 3:
          SPI.beginTransaction(settingsD);
          break;
        default:
          break;
      }
      digitalWrite(10, LOW);
      SPI.transfer(0x03);
      SPI.transfer(0);
      SPI.transfer(0);
      SPI.transfer(0);
      for (unsigned long i = 0; i < flashsize; i++) {
        tempbuffer = SPI.transfer(0);
        Serial.write(tempbuffer);
      }
      digitalWrite(10, HIGH);
      SPI.endTransaction(); // End Mode 0
    } else if (input[0] == 1) { //Mode 1: Byte-by-byte read. usage: bytearray([MODE, READ_SIZE, ADDR_UPPER, ADDR_MIDDLE, ADDR_LOWER, IGNORE, SPISETTING])
      switch (input[6]) {
        case 0:
          SPI.beginTransaction(settingsA);
          break;
        case 1:
          SPI.beginTransaction(settingsB);
          break;
        case 2:
          SPI.beginTransaction(settingsC);
          break;
        case 3:
          SPI.beginTransaction(settingsD);
          break;
        default:
          break;
      }
      upper = input[2];
      middle = input[3];
      lower = input[4];
      for (int i = 0; i <= input[1]; i++) {
        digitalWrite(10, LOW);
        SPI.transfer(0x03);
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
      SPI.endTransaction(); // End Mode 1
    } else if (input[0] == 2) { // Mode 2: Custom command. usage: bytearray([MODE, SPI_COMMAND, SPI_BYTE0, SPI_BYTE1, SPI_BYTE2, SPI_BYTE3, SPISETTING])
      switch (input[6]) {
        case 0:
          SPI.beginTransaction(settingsA);
          break;
        case 1:
          SPI.beginTransaction(settingsB);
          break;
        case 2:
          SPI.beginTransaction(settingsC);
          break;
        case 3:
          SPI.beginTransaction(settingsD);
          break;
        default:
          break;
      }

      if (input[1] != 0x52 && input[1] != 0xD8 && input[1] != 0x60 && input[1] != 0xC7 && input[1] != 0x02 && input[1] != 0x2F) {
        digitalWrite(10, LOW);
        SPI.transfer(input[1]);
        if (input[1] == 0x03 || input[1] == 0x0B || input[1] == 0x5A || input[1] == 0xAB || input[1] == 0x90 || input[1] == 0x3B || input[1] == 0x20) {
          SPI.transfer(input[2]);
          SPI.transfer(input[3]);
          SPI.transfer(input[4]);
          if (input[1] == 0x0B || input[1] == 0x5A || input[1] == 0x3B) {
            SPI.transfer(input[5]);
          }
        }
        if (input[1] != 0x06 && input[1] != 0x04 && input[1] != 0x01 && input[1] != 0xB1 && input[1] != 0xC1 && input[1] != 0xB9 && input[1] != 0xAB && input[1] != 0x20) {
          data = SPI.transfer(0);
          Serial.write(data);
          if (input[1] == 0x90 || input[1] == 0x9F) {
            data = SPI.transfer(0);
            Serial.write(data);
            if (input[1] == 0x9F) {
              data = SPI.transfer(0);
              Serial.write(data);
            }
          }
        }
        digitalWrite(10, HIGH);
      } else {
        Serial.write(0xB);
        Serial.write(0xAD);
        Serial.write(0xC0);
        Serial.write(0xDE);
        Serial.write(0xB);
        Serial.write(0xAD);
        Serial.write(0xC0);
        Serial.write(0xDE);
        Serial.write(0xB);
        Serial.write(0xAD);
        Serial.write(0xC0);
        Serial.write(0xDE);
      }
      SPI.endTransaction(); // End Mode 2
    } else if (input[0] == 3) { // Mode 3: Page program. usage: bytearray([MODE, ADDR_UPPER, ADDR_MIDDLE, ADDR_LOWER, NO_VERIFY, WRITE_SIZE, SPISETTING])
      if (input[4] == 0) {
        receive_input();
        Serial.write(page_chunk0, 64);
        Serial.write(page_chunk1, 64);
        Serial.write(page_chunk2, 64);
        Serial.write(page_chunk3, 64);
        serial_flush();
        while (true) {
          if (Serial.available()) {
            Serial.readBytes(&tempbuffer, 1);
            serial_flush();
            if (tempbuffer == 0x55) {
              write_flash();
              break;
            } else {
              break;
            }
          }
        }
      } else if (input[4] == 1) {
        receive_input();
        write_flash();
      }
    } // End Mode 3
    else if (input[0] == 4) { // Mode 4: Byte write. usage: bytearray([MODE, ADDR_UPPER, ADDR_MIDDLE, ADDR_LOWER, DATA, IGNORE, SPISETTING])
      switch (input[6]) {
        case 0:
          SPI.beginTransaction(settingsA);
          break;
        case 1:
          SPI.beginTransaction(settingsB);
          break;
        case 2:
          SPI.beginTransaction(settingsC);
          break;
        case 3:
          SPI.beginTransaction(settingsD);
          break;
        default:
          break;
      }
      digitalWrite(10, LOW);
      SPI.transfer(0x06); // Write Enable command
      digitalWrite(10, HIGH);

      digitalWrite(10, LOW);
      SPI.transfer(0x05); // Read Status Register command
      statusregister = SPI.transfer(0); // Status register
      digitalWrite(10, HIGH);

      if ((statusregister & WEL) && !(statusregister & WIP)) {
        digitalWrite(10, LOW);
        SPI.transfer(0x02);
        SPI.transfer(input[1]);
        SPI.transfer(input[2]);
        SPI.transfer(input[3]);
        SPI.transfer(input[4]);
        digitalWrite(10, HIGH);
        SPI.endTransaction();
      } else {
        Serial.println("Cannot write to flash");
        SPI.endTransaction();
      }
    } else if (input[0] == 5) { // Mode 5: Sector Erase. usage: bytearray([MODE, ADDR_UPPER, ADDR_MIDDLE, ADDR_LOWER, IGNORE, IGNORE, SPISETTING])
      switch (input[6]) {
        case 0:
          SPI.beginTransaction(settingsA);
          break;
        case 1:
          SPI.beginTransaction(settingsB);
          break;
        case 2:
          SPI.beginTransaction(settingsC);
          break;
        case 3:
          SPI.beginTransaction(settingsD);
          break;
        default:
          break;
      }
      digitalWrite(10, LOW);
      SPI.transfer(0x06); // Write Enable command
      digitalWrite(10, HIGH);

      digitalWrite(10, LOW);
      SPI.transfer(0x05); // Read Status Register command
      statusregister = SPI.transfer(0); // Status register
      digitalWrite(10, HIGH);

      if ((statusregister & WEL) && !(statusregister & WIP)) {
        digitalWrite(10, LOW);
        SPI.transfer(0x20);
        SPI.transfer(input[1]);
        SPI.transfer(input[2]);
        SPI.transfer(input[3]);
        digitalWrite(10, HIGH);
        SPI.endTransaction();
      } else {
        Serial.println("Cannot erase sector");
        SPI.endTransaction();
      }
    } else if (input[0] == 6) { // Mode 6: Block Erase. usage: bytearray([MODE, ADDR_UPPER, ADDR_MIDDLE, ADDR_LOWER, IGNORE, IGNORE, SPISETTING])
      switch (input[6]) {
        case 0:
          SPI.beginTransaction(settingsA);
          break;
        case 1:
          SPI.beginTransaction(settingsB);
          break;
        case 2:
          SPI.beginTransaction(settingsC);
          break;
        case 3:
          SPI.beginTransaction(settingsD);
          break;
        default:
          break;
      }
      digitalWrite(10, LOW);
      SPI.transfer(0x06); // Write Enable command
      digitalWrite(10, HIGH);

      digitalWrite(10, LOW);
      SPI.transfer(0x05); // Read Status Register command
      statusregister = SPI.transfer(0); // Status register
      digitalWrite(10, HIGH);

      if ((statusregister & WEL) && !(statusregister & WIP)) {
        digitalWrite(10, LOW);
        SPI.transfer(0x52);
        SPI.transfer(input[1]);
        SPI.transfer(input[2]);
        SPI.transfer(input[3]);
        digitalWrite(10, HIGH);
        SPI.endTransaction();
      } else {
        Serial.println("Cannot erase block");
        SPI.endTransaction();
      }
    } else {
      Serial.println("Invalid");
    }
  }
}
