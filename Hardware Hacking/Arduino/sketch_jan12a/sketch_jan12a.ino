/* Learn how to use a potentiometer to fade an LED with Arduino - Tutorial
   More info and circuit schematic: http://www.ardumotive.com/arduino-tutorials/arduino-fade-led
   Dev: Michalis Vasilakis / Date: 25/10/2014                                                   */


//Constants:
const int pwmPin = 9;  //pin 9 has PWM funtion
const int potPin = A0; //pin A0 to read analog input
const int counterButton = 2;     // the number of the pushbutton pin
const int resetButton = 3;
const int ledPin = 13;
const int ledPin2 = 4;

//Variables:
int value = 0; //save analog value
int turns = 0;
int buttonState = 0;
bool isValidCount = true;
bool resetNotPressed = true;

void setup() {
  //Input or output?
  pinMode(ledPin, OUTPUT);
  pinMode(potPin, INPUT); //Optional
  pinMode(counterButton, INPUT);
  pinMode(resetButton, INPUT);
  pinMode(ledPin, OUTPUT);
  pinMode(ledPin2, OUTPUT);
  Serial.begin(115200);

}

void loop() {
  // read the state of the pushbutton value:
  buttonState = digitalRead(counterButton);

  if (digitalRead(resetButton) == HIGH){
    if(resetNotPressed){
      Serial.print("RESET\n");
      digitalWrite(ledPin, HIGH);
      delay(1000);
      digitalWrite(ledPin, LOW);
      delay(100);
      digitalWrite(ledPin, HIGH);
      delay(100);
      digitalWrite(ledPin, LOW);
      delay(100);
      digitalWrite(ledPin, HIGH);
      delay(100);
      digitalWrite(ledPin, LOW);
      turns = 0;
      resetNotPressed = false;
    }
  } else {
    resetNotPressed = true;
  }

  // check if the pushbutton is pressed. If it is, the buttonState is HIGH:
  if (buttonState == HIGH) {
    if (isValidCount) {
      digitalWrite(ledPin2, HIGH);
      Serial.println(turns);
      turns += 1;
      isValidCount = false;
      digitalWrite(ledPin2, LOW);
    }
  } else {
    isValidCount = true;
  }

  value = analogRead(pwmPin);          //Read and save analog value from potentiometer
  value = map(value, 0, 1023, 0, 255); //Map value 0-1023 to 0-255 (PWM)
  if (turns <= 100) {
    analogWrite(pwmPin, value);          //Send PWM value to led
  } else {
    isValidCount = false;
    analogWrite(pwmPin, 0);
  }
}
