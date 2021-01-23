
const int inputPin1 = 2;
const int inputPin2 = 3;
const int inputPin3 = 4;
const int inputPin4 = 5;
const int inputPin5 = 6;
const int inputPin6 = 7;
const int inputPin7 = 8;
const int ledPin    = 9;

void setup() {
  pinMode(inputPin1, INPUT);
  pinMode(inputPin2, INPUT);
  pinMode(inputPin3, INPUT);
  pinMode(inputPin4, INPUT);
  pinMode(inputPin5, INPUT);
  pinMode(inputPin6, INPUT);
  pinMode(inputPin7, INPUT);
  pinMode(ledPin, OUTPUT);
}

void loop() {

  int pin1 = digitalRead(inputPin1);
  int pin2 = digitalRead(inputPin2);
  int pin3 = digitalRead(inputPin3);
  int pin4 = digitalRead(inputPin4);
  int pin5 = digitalRead(inputPin5);
  int pin6 = digitalRead(inputPin6);
  int pin7 = digitalRead(inputPin7);

  int x = 0;
  if (pin1 == LOW) {
    x = 1;
  }
  if (pin2 == LOW) {
    x = 2;
  }
  if (pin3 == LOW) {
    x = 3;
  }
  if (pin4 == LOW) {
    x = 4;
  }
  if (pin5 == LOW) {
    x = 5;
  }
  if (pin6 == LOW) {
    x = 6;
  }
  if (pin7 == LOW) {
    x = 7;
  }

  if (x == 0) {
    digitalWrite(ledPin, 1);
  } else {
    for (int i=0; i<x; i++) {
      digitalWrite(ledPin, 0);
      delay(250);
      digitalWrite(ledPin, 1);
      delay(250);
    }
  }

  delay(500);
}
