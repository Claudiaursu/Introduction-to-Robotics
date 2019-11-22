const int pinA = 12;
const int pinB = 8;
const int pinC = 5;
const int pinD = 3;
const int pinE = 2;
const int pinF = 11;
const int pinG = 6;
const int pinDP = 4;
const int pinD1 = 7;
const int pinD2 = 9;
const int pinD3 = 10;
const int pinD4 = 13;

const int pinX = A0; //analog input, there s no need to declare in pinMode
const int pinY = A1; //analog input
const int pinSW = A2;
int valueX = 0;
int valueY = 0;
int valueSW = 0;

int blinkState = HIGH;
int dpState = HIGH;
bool joyMoved = false;
bool digitLocked = false; // to check whether the digit is locked or not
int lockedBefore = false;
int digitToShow = 0; // the initial digit from where we start is the one from the left
int minThreshold = 400; // min value of the default joystick state
int maxThreshold = 600; // max value of the default joystick state

int lastMillis = 0;

const int segSize = 8;
const int noOfDisplays = 4;
const int noOfDigits = 10;

int segments[segSize] = {
  pinA, pinB, pinC, pinD, pinE, pinF, pinG, pinDP
};

int digits[noOfDisplays] = {
  pinD1, pinD2, pinD3, pinD4
 };

int digitValues[noOfDisplays]={
  0,0,0,0
  };

byte dpStates[noOfDisplays] = {
  LOW,LOW,LOW,LOW
};

 byte digitMatrix[noOfDigits][segSize - 1] = {
// a  b  c  d  e  f  g
  {1, 1, 1, 1, 1, 1, 0}, // 0
  {0, 1, 1, 0, 0, 0, 0}, // 1
  {1, 1, 0, 1, 1, 0, 1}, // 2
  {1, 1, 1, 1, 0, 0, 1}, // 3
  {0, 1, 1, 0, 0, 1, 1}, // 4
  {1, 0, 1, 1, 0, 1, 1}, // 5
  {1, 0, 1, 1, 1, 1, 1}, // 6
  {1, 1, 1, 0, 0, 0, 0}, // 7
  {1, 1, 1, 1, 1, 1, 1}, // 8
  {1, 1, 1, 1, 0, 1, 1}  // 9
};


// display the number received as parameter, named digit
void displayNumber(byte digit, byte decimalPoint) {
  for (int i = 0; i < segSize - 1; i++) {
  digitalWrite(segments[i], digitMatrix[digit][i]);
  }
    // write the decimalPoint to DP pin
  digitalWrite(segments[segSize - 1], decimalPoint);
}

// activate the display no. received as param
void showDigit(int num) {
  for (int i = 0; i < noOfDisplays; i++) {
  digitalWrite(digits[i], HIGH);
  }
  digitalWrite(digits[num], LOW);
}

void blinkDP(int num) {
  if(millis() - lastMillis > 500) {
    lastMillis = millis();
    blinkState = ! blinkState;
    digitalWrite(segments[segSize - 1], LOW);
  }
}
void setup() {
    for (int i = 0; i < segSize - 1; i++)
  {
  pinMode(segments[i], OUTPUT);
  }
  for (int i = 0; i < noOfDisplays; i++)
  {
  pinMode(digits[i], OUTPUT);
  }
  pinMode(pinDP, OUTPUT);
  pinMode(pinSW,INPUT_PULLUP);
}

void loop() {
  valueX = analogRead(pinX);
  valueY = analogRead(pinY);
  valueSW = digitalRead(pinSW);

  if(valueY < minThreshold && joyMoved == false && digitLocked == false) {
    if(digitToShow < 3)
    digitToShow++;
    else {
      digitToShow = 0;
    }
    blinkDP(digitToShow);
    showDigit(digitToShow);
    joyMoved = true;
  }
  if(valueY > maxThreshold && joyMoved == false && digitLocked == false) {
   if(digitToShow > 0)
    digitToShow--;
    else {
      digitToShow = 3;
    }
    blinkDP(digitToShow);
    showDigit(digitToShow);
    joyMoved = true;
  }
  
  if(valueY <= maxThreshold && valueY >= minThreshold && digitLocked == false)
  joyMoved = false;

  if(valueX > maxThreshold && joyMoved == false && digitLocked == true) {
    digitValues[digitToShow] ++;
    joyMoved = true;
    if(digitValues[digitToShow] > 9) digitValues[digitToShow] = 0;
  }
  
  if(valueX < minThreshold && joyMoved == false && digitLocked == true){
    digitValues[digitToShow] --;
    joyMoved = true;
    if(digitValues[digitToShow] < 0) digitValues[digitToShow] = 9;
    digitValues[digitToShow] = digitValues[digitToShow];
  }

  if(valueX <= maxThreshold && valueX >= minThreshold && valueY <= maxThreshold && valueY >= minThreshold && digitLocked == true) {
   joyMoved = false;
    valueSW = digitalRead(pinSW);
    if(valueSW == 0) {
      digitLocked = !digitLocked;
    }
  }

  else if(valueX <= maxThreshold && valueX >= minThreshold && digitLocked == false) {
   joyMoved = false;
    valueSW = digitalRead(pinSW);
    if(valueSW == 0) {
      digitLocked = ! digitLocked;
      Serial.println(valueSW);
    }
  }

 displayNumber(digitValues[digitToShow], dpState);
 showDigit(digitToShow);
 delay(400);
}
