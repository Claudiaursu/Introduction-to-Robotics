#include<LiquidCrystal.h>
const int RS = 12;
const int E = 11;
const int D4 = 5;
const int D5 = 4;
const int D6 = 3;
const int D7 = 2;
const int pinX = A0; 
const int pinY = A1; 
const int pinSW = 9; 

int valueX = 0;
int valueY = 0;
int valueSW = 0;
const int minThreshold = 400;
const int maxThreshold = 600;
bool joyMoved = false;

char displayState = 'm';
char currentOption = 'p';
LiquidCrystal lcd(RS,E,D4,D5,D6,D7);

bool SWPressed = false;

int startingLevelValue = 0;
int currentTimeOnPlayClick = 0;
int highScore = 0;
int currentLevel = 0;

int currentScore = 0;

void setup() {
  lcd.begin(16,2); //16 for columns and 2 for rows
  lcd.clear(); //to erase evrithing on the screen
  pinMode(pinSW,INPUT_PULLUP);
  Serial.begin(9600);
}
 void mainMenuScreen() {
  lcd.setCursor(1,0);
  lcd.print("Play");
  lcd.setCursor(7,0);
  lcd.print("HighScore"); 
  lcd.setCursor(1,1);
  lcd.print("Settings"); 

  valueX = analogRead(pinX);
  valueY = analogRead(pinY);
  valueSW = digitalRead(pinSW);

  if (valueX > minThreshold && valueX < maxThreshold && valueY > minThreshold && valueY < maxThreshold) {
    joyMoved = false;
  }
  
  if (valueY < minThreshold && currentOption == 'h') {
    currentOption = 'p';
    joyMoved = true;
  }
  if (valueY > maxThreshold && currentOption == 'p') {
    currentOption = 'h';
    joyMoved = true;
  }
  if (valueX > maxThreshold && currentOption == 's') {
    currentOption = 'p';
    joyMoved = true;
  }
  if (valueX < minThreshold && currentOption != 's') {
    currentOption = 's';
    joyMoved = true;
  }

  if (valueSW == LOW && !SWPressed) {
    SWPressed = true;
    
    if (currentOption == 'p') {
      currentTimeOnPlayClick = millis();
      displayState = 'p';
    }
    else if (currentOption == 'h') {
      displayState = 'h';
    }
    else if (currentOption == 's') {
      displayState = 's';
    }
  }
  
  showCursor(currentOption);
 }

void showCursor(char option) {
  if(option == 'p') {
    lcd.setCursor(0,0);
    lcd.print(">");
    lcd.setCursor(6,0);
    lcd.print(" ");
    lcd.setCursor(0,1);
    lcd.print(" ");
  }
   if(option == 'h') {
    lcd.setCursor(6,0);
    lcd.print(">");
    lcd.setCursor(0,0);
    lcd.print(" ");
    lcd.setCursor(0,1);
    lcd.print(" ");
  }
    if(option == 's') {
      lcd.setCursor(0,1);
      lcd.print(">");
      lcd.setCursor(6,0);
      lcd.print(" ");
      lcd.setCursor(0,0);
      lcd.print(" ");
  } 
}

void playScreen() {

  currentLevel = startingLevelValue;
  currentScore = 0;
  lcd.setCursor(0,0);
  lcd.print("Lives:3  Lvl:");
  lcd.setCursor(13,0);
  lcd.print(currentLevel);
  lcd.setCursor(0,1);
  lcd.print("Score: ");
  lcd.setCursor(7,1);
  lcd.print(currentScore);

  // update level and the score
  if (millis() - currentTimeOnPlayClick > 2000) {
    currentLevel++;
    currentScore = currentLevel * 3;
    
  }

  if (millis() - currentTimeOnPlayClick > 10000) {
   displayState = 'f';

    if (currentScore > highScore) {
      highScore = currentScore;
    }
  }
}

void highScoreScreen() {
  valueSW = digitalRead(pinSW);

  lcd.setCursor(0,0);
  lcd.print("High score:");
  lcd.setCursor(11,0);
  lcd.print(highScore);
  
  if (valueSW == LOW && !SWPressed) {
    SWPressed = true;
    
    displayState = 'm';
  }
}

void finishScreen() {
  lcd.setCursor(0,0);
  lcd.print("Congratulations!");
  lcd.setCursor(0,1);
  lcd.print("Press JS to exit");

  valueSW=digitalRead(pinSW);
  if (valueSW == LOW && !SWPressed) {
    SWPressed = true;
    
    displayState = 'm';
  }
}

void settingsScreen() {
  valueSW = digitalRead(pinSW);
  valueX = analogRead(pinX);
  
  if (valueX > minThreshold && valueX < maxThreshold && valueY > minThreshold && valueY < maxThreshold) {
    joyMoved = false;
  }
  
  if (!joyMoved && valueX > maxThreshold) {
    if (startingLevelValue >= 9) {
      startingLevelValue = 0;
    }
    else {
      startingLevelValue++;
    }
    joyMoved = true;
  }
  if (!joyMoved && valueX < minThreshold) {
    if (startingLevelValue <= 0) {
      startingLevelValue = 9;
    }
    else {
      startingLevelValue--;
    }
    joyMoved = true;
  }

  lcd.setCursor(0,0);
  lcd.print("Starting lvl:");
  lcd.setCursor(15,0);
  lcd.print(startingLevelValue);
  
  if (valueSW == LOW && !SWPressed) {
    SWPressed = true;
    displayState = 'm';
  }
}

void loop() {
  valueSW=digitalRead(pinSW);

  if (SWPressed) {
    lcd.clear();
  }
  if (valueSW == HIGH) {
     SWPressed = false;
  }

  if(displayState == 'm') {
     mainMenuScreen();
  }
  else if(displayState == 'p') {
    currentLevel = startingLevelValue;
    playScreen();
  }
  else if(displayState == 's') {
    settingsScreen();
  }
  else if(displayState == 'h') {
    highScoreScreen();
  }
  else if(displayState == 'f') {
    finishScreen();
  }
  
  }
