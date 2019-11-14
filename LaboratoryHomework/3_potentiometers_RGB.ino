const int potPin1 = A0;
const int potPin2 = A1;
const int potPin3 = A2;

const int ledPin1 = 3;
const int ledPin2 = 9;
const int ledPin3 = 6;
int potValue1 = 0, ledValue1 = 0;
int potValue2 = 0, ledValue2 = 0;
int potValue3 = 0, ledValue3 = 0;

void setup() {
  
pinMode(potPin1, INPUT);
pinMode(potPin2, INPUT);
pinMode(potPin3, INPUT);
pinMode(ledPin1, OUTPUT);
pinMode(ledPin2, OUTPUT);
pinMode(ledPin3, OUTPUT);

}

void loop() {
  
  potValue1 = analogRead(potPin1);
  ledValue1 = map(potValue1, 0, 1023, 0, 255);
  analogWrite(ledPin1, ledValue1);

  potValue2 = analogRead(potPin2);
  ledValue2 = map(potValue2, 0, 1023, 0, 255);
  analogWrite(ledPin2, ledValue2);

  potValue3 = analogRead(potPin3);
  ledValue3 = map(potValue3, 0, 1023, 0, 255);
  analogWrite(ledPin3, ledValue3);
  
}
