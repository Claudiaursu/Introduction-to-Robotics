#include <LinkedListLib.h>
#include<LedControl.h>
LedControl lc = LedControl(12,11,10,1); //DIN, CLK, LOAD, number of driver
#include<LiquidCrystal.h>
const int RS = 7;
const int E = 8;
const int D4 = 5;
const int D5 = 4;
const int D6 = 3;
const int D7 = 2;
LiquidCrystal lcd(RS,E,D4,D5,D6,D7);

bool SWPressed = false;
int currentMillis;
int previousMillis = 0;
bool valBlink = true;

int startingLevelValue = 1;
long int startGameTime = 0;
char currentOption = 'p';
char currentOptionSettings = 'l';
char currentInfoOption = 'a';
char settingsScreenToShow = 'x';
char infoScreenToShow = 'x';
char screenToShow = 'm';

const int joyX = A1;
const int joyY = A0;
const int joySW = 9;

bool isPlaying = false;
bool gameOver = false;
int lives = 3;

const int pinLed = 13;
int ledState = LOW;

int intervalMillis = 500;
int level = 1;

int headX = 0, headY = 1;
int endX = 0, endY = 0;
int valX, valY, valSW;
boolean movedX = false;
boolean movedY = false;
boolean joyMoved = false;
char currentDirection = 'N';
char nextDirection = 'N';
long int  lastPlacedFoodMillis = 0;

int pointsEaten = 0;
int score = 0;
int highScore = 0;

int snake[64][2] = {
{headX, headY}, {endX, endY}
};
int snakeLength = 2;
int foodX = 5;
int foodY = 2;
long int lastMillis = 0;

struct Coordinate {
  int x;
  int y;
};

/* in this structure we save all the available positions for the food to be placed( without the body of the snake at that time) */
LinkedList <Coordinate> foodAvailablePositions; 

void setup() {
 lc.shutdown(0,false); //first driver turned on all the time
 lc.setIntensity(0,0.5); //intensity is from 0-15
 lc.clearDisplay(0); //clears the display on the first driver (the only driver used in this game)
 pinMode(pinLed,OUTPUT);
 Serial.begin(9600);
 lc.setLed(0,foodX,foodY,true);

  lcd.begin(16,2); 
  lcd.clear(); 
  lcd.setCursor(0,0); 
  pinMode(joySW,INPUT_PULLUP);
}

// this is the logo of the game, showed when the player goes through the lcd menu
int notPlayingImage[8][8] = {
  {0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0},
  {0,1,1,1,1,1,1,0},
  {0,0,0,0,0,0,1,0},
  {0,0,1,0,0,1,1,0},
  {0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0}
};

// this image will show everytime 1 life is lost
int lifeLossImage[8][8] = {
  {1,0,0,0,0,0,1,1},
  {1,1,0,0,0,0,1,1},
  {0,1,1,0,0,1,1,0},
  {0,0,1,1,1,1,0,0},
  {0,0,0,1,1,0,0,0},
  {0,0,1,1,1,1,0,0},
  {0,1,1,0,0,1,1,0},
  {1,1,0,0,0,0,1,1}
};

void clearMatrix(int x, int y) {
  for(int i = 0; i < 8; i++)
   for(int j = 0; j < 8; j++) {
    lc.setLed(0,i,j,false);
   }

   lc.setLed(0,x,y,true);
}

void clearSnake( int snakeToMove[64][2]) {
  for(int i = 0; i <= snakeLength; i++) {
      lc.setLed(0,snakeToMove[i][0],snakeToMove[i][0],false);
    }
}

void placeFood(int snakeToMove[64][2]) {
  // Generate all possible points to display a food point
  foodAvailablePositions.Clear();
 for (int x = 0; x < 8; x++) {
  for (int y = 0; y < 8; y++) {
    Coordinate coord;
    coord.x = x;
    coord.y = y;
    foodAvailablePositions.InsertTail(coord);
  }
 }
 for(int i = 0; i < snakeLength; i++) {
  // in an auxiliary structure we save all the values of the body of the snake
  Coordinate auxCoord;
  auxCoord.x = snakeToMove[i][0];
  auxCoord.y = snakeToMove[i][1];

/* eliminate the positions of the body points, so that there would be no chance for the new food point to be placed 
  on the body of the snake */
  for(int j = 0; j < foodAvailablePositions.GetSize(); j++) {
    if(auxCoord.x == foodAvailablePositions.GetAt(j).x  && auxCoord.y == foodAvailablePositions.GetAt(j).y) {
      foodAvailablePositions.RemoveAt(j);
      break;
    }
  }
 }
 // eliminate the posibility of getting a random position for the new food, that would be the same as the last position
 for(int j = 0; j < foodAvailablePositions.GetSize(); j++) {
    if(foodX == foodAvailablePositions.GetAt(j).x  && foodY == foodAvailablePositions.GetAt(j).y) {
      foodAvailablePositions.RemoveAt(j);
      break;
    }
  }
  
 /* generate a random index in the structure of available positions for the food */
 int randIndex = random(0,foodAvailablePositions.GetSize()); 
 Coordinate coordFinal; // an auxiliary structure for the final position of the food
 coordFinal = foodAvailablePositions.GetAt(randIndex);
 foodX=coordFinal.x;
 foodY=coordFinal.y;
 lc.setLed(0,foodX, foodY, true); // light up the new food point
}

bool isDead(int snakeToDie[62][2]) {
  for(int i = 1; i < snakeLength; i++) {
    if(snakeToDie[0][0] == snakeToDie[i][0] && snakeToDie[0][1] == snakeToDie[i][1] ) {
      return true;
    }
  }
  return false;
}

void generalMove(int snakeToMove[64][2]) {
  for(int i = snakeLength; i >= 0; i--) {
    snakeToMove[i][1] = snakeToMove[i-1][1];
    snakeToMove[i][0] = snakeToMove[i-1][0];
  }
  snakeToMove[0][0] = headX;
  snakeToMove[0][1] = headY;
  lc.setLed(0,snakeToMove[snakeLength][0],snakeToMove[snakeLength][1],false);
}

void moveUp(int snakeToMove[64][2]) {
  headY++;
  if(headY == 8) headY = 0;
  generalMove(snakeToMove);
}

void moveRight(int snakeToMove[64][2]) {
   headX++;
  if(headX == 8) headX = 0;
  generalMove(snakeToMove); 
}

void moveLeft(int snakeToMove[64][2]) {
  headX--;
  if(headX == -1) headX = 7;
  generalMove(snakeToMove); 
}

void moveDown(int snakeToMove[64][2]) {
   headY--;
  if(headY == -1) headY = 7;
  generalMove(snakeToMove); 
}

void lightLed(int lightTime) {
  ledState = HIGH;
  if(millis() - lightTime >= 1000) {
    Serial.println(lightTime);
    ledState = LOW;
  }
   digitalWrite(pinLed,ledState); 
}

/* this function checks the movement of the snake, covering all the cases when a snake could eat a food point (when comming
from North, East, West, or South). It also updates the value of the score and level */
void playGame() {
   if(level == 3 && millis() - lastPlacedFoodMillis > 2000) {
   lastPlacedFoodMillis = millis();   
   lc.setLed(0,foodX, foodY, false);        
     placeFood(snake);
   }
  
   valX = analogRead(joyX);
   valY = analogRead(joyY);
  
   if(valY > 600 && joyMoved == false) {
   
    joyMoved = true;
    if(currentDirection != 'E') {
      nextDirection = 'W';
    }
   }
   else joyMoved = false;
   
   if(valY < 400 && joyMoved == false) {
   if(currentDirection != 'W') {
      nextDirection = 'E';
    }
    joyMoved = true;
   }
   else joyMoved = false;
   
   if(valX < 400 && joyMoved == false) {
     if(currentDirection != 'S') {
      nextDirection = 'N';
    }
    joyMoved = true;
   }
   else joyMoved = false;
   
   if(valX > 600 && joyMoved == false) {
    if(currentDirection != 'N') {
      nextDirection = 'S';
    }
    joyMoved = true;
   }
   else joyMoved = false;
  
  if(millis()- lastMillis > intervalMillis) {
    lastMillis = millis();
    currentDirection = nextDirection;
    switch(nextDirection) {
    case 'N' : {
       if(foodX == snake[0][0] && foodY == snake[0][1]+1) {
        snakeLength++;
        pointsEaten++;
        score = pointsEaten*2*level;
        ledState = HIGH;
        long int ledStart = millis();
        while(millis() - ledStart < 500) {
          digitalWrite(pinLed,ledState);
        }
        ledState = LOW;
        digitalWrite(pinLed,ledState);
         
        if (pointsEaten == 5) {
          level = 2;
          intervalMillis = 250;
        }
        if(pointsEaten == 12) {
          level = 3;
          lastPlacedFoodMillis = millis();
          intervalMillis = 200;
        }
        placeFood(snake);
       }
      moveUp(snake);
      break;
    }
      case 'E' : {
          if(foodX == snake[0][0]+1 && foodY == snake[0][1]) {
          snakeLength++;
          pointsEaten++;
          score = pointsEaten*2*level;
          
          ledState = HIGH;
          long int ledStart = millis();
          while(millis() - ledStart < 500) {
            digitalWrite(pinLed,ledState);
          }
          ledState = LOW;
          digitalWrite(pinLed,ledState);

          if (pointsEaten == 5) {
            level = 2;
            intervalMillis = 250;
          }
          
          if(pointsEaten == 12) {
            level = 3;
            lastPlacedFoodMillis = millis();
            intervalMillis = 200;
          }
          
            placeFood(snake);
         }
          moveRight(snake);
          break;
      }
      case 'S' : {
        if(foodX == snake[0][0] && foodY == snake[0][1]-1) {
          snakeLength++;
          pointsEaten++;
          score = pointsEaten*2*level;
          ledState = HIGH;
          long int ledStart = millis();
          while(millis() - ledStart < 500) {
            digitalWrite(pinLed,ledState);
          }
          ledState = LOW;
          digitalWrite(pinLed,ledState);
                
          if (pointsEaten == 5) {
            level = 2;
            intervalMillis = 250;
          }
          if(pointsEaten == 12) {
            level = 3;
            lastPlacedFoodMillis = millis();
            intervalMillis = 200;
          }
          placeFood(snake);
         }
          moveDown(snake);
          break;
      }
     case 'W' : {
       if(foodX == snake[0][0]-1 && foodY == snake[0][1]) {
          snakeLength++;
          pointsEaten++;
          score = pointsEaten*2*level;
          ledState = HIGH;
          long int ledStart = millis();
          while(millis() - ledStart < 500) {
            digitalWrite(pinLed,ledState);
          }
          ledState = LOW;
          digitalWrite(pinLed,ledState);

          Serial.println(pointsEaten);

          if (pointsEaten == 5) {
            level = 2;
            intervalMillis = 250;
          }
          if(pointsEaten == 12) {
            level = 3;
            lastPlacedFoodMillis = millis();
            intervalMillis = 200;
          }
          placeFood(snake);
         }
          moveLeft(snake);
          break;
      }   
     }
      
      if(isDead(snake)) {
       lives--;
       lifeLossDisplay(lifeLossImage, millis(), foodX, foodY);
       lc.clearDisplay(0);
       lc.setLed(0,foodX,foodY,true);
       
       if(lives == 0) {
        isPlaying = false; 
        gameOver = true;
        if(score > highScore) {
          highScore = score;
        }
       }
      }
      for(int i = 0; i < snakeLength; i++)
       lc.setLed(0,snake[i][0],snake[i][1],true);
        
    }
}

void displayDeadScreen() {
  for(int row = 0; row < 8; row++)
 for(int col = 0; col < 8; col++)
 {
  lc.setLed(0,col,row,true);
  delay(45);
 }

 for(int row = 0; row < 8; row++)
 for(int col = 0; col < 8; col++)
 {
  lc.setLed(0,col,row,false);
 delay(25);
 }
}

void lifeLossDisplay(int m[8][8], long int displayStart, int x, int y)
{   lc.clearDisplay(0);
   while(millis() - displayStart < 1500) {
   clearSnake(snake[64][2]);
   for(int j = 0; j < 8; j++)
   for(int i = 0; i < 8; i++){
   if(m[i][j] != 0)
   lc.setLed(0,j,i,true);
   }
   }
   clearMatrix(x, y);
}

void imageDisplay(int m[8][8])
{   
   clearSnake(snake[64][2]);
   for(int j = 0; j < 8; j++)
   for(int i = 0; i < 8; i++){
   if(m[i][j] != 0)
   lc.setLed(0,i,j,true);
   }
}

void showCursor(char option) {
   if(option == 'p') {
    lcd.setCursor(0,0);
    lcd.print(">");
    lcd.setCursor(6,0);
    lcd.print(" ");
    lcd.setCursor(0,1);
    lcd.print(" ");
    lcd.setCursor(11,1);
    lcd.print(" ");
  }
   if(option == 'h') {
    lcd.setCursor(0,1);
    lcd.print(">");
    lcd.setCursor(0,0);
    lcd.print(" ");
    lcd.setCursor(6,0);
    lcd.print(" ");
    lcd.setCursor(11,1);
    lcd.print(" ");
  }
    if(option == 's') {
    lcd.setCursor(6,0);
    lcd.print(">");
    lcd.setCursor(0,1);
    lcd.print(" ");
    lcd.setCursor(0,0);
    lcd.print(" ");
    lcd.setCursor(11,1);
    lcd.print(" ");
  } 
    if(option == 'i') {
    lcd.setCursor(11,1);
    lcd.print(">");
    lcd.setCursor(6,0);
    lcd.print(" ");
    lcd.setCursor(0,0);
    lcd.print(" ");
    lcd.setCursor(0,1);
    lcd.print(" ");
  }
}

// this function shows the relevant information about the game(score, number of lives, level), while the player is playing
void playingScreen() {
  lcd.setCursor(0,0);
  lcd.print("Score:");
  lcd.setCursor(6,0);
  lcd.print(score);
  lcd.setCursor(9,1);
  lcd.print("Lives:");
  lcd.setCursor(15,1);
  lcd.print(lives);
  lcd.setCursor(0,1);
  lcd.print("Level:");
  lcd.setCursor(6,1);
  lcd.print(level);
}

void gameOverScreen(int score){
  lcd.setCursor(0,0);
  lcd.print("Your score:");
  lcd.setCursor(11,0);
  lcd.print(score);
  lcd.setCursor(0,1);
  lcd.print("Press SW-go back");
}

void highScoreScreen(int score){
  lcd.setCursor(0,0);
  lcd.print("NEWHighScore:");
  lcd.setCursor(13,0);
  lcd.print(score);
  lcd.setCursor(0,1);
  lcd.print("Press SW-go back");
}

void showCursorForSettings(int option){
   if(option == 'l') {
    lcd.setCursor(0,0);
    lcd.print(">");
    lcd.setCursor(0,1);
    lcd.print(" ");
    lcd.setCursor(10,1);
    lcd.print(" ");
  }
   if(option == 'c') {
    lcd.setCursor(0,1);
    lcd.print(">");
    lcd.setCursor(0,0);
    lcd.print(" ");
    lcd.setCursor(10,1);
    lcd.print(" ");
  }
   if(option == 'b') {
    lcd.setCursor(0,0);
    lcd.print(" ");
    lcd.setCursor(0,1);
    lcd.print(" ");
    lcd.setCursor(10,1);
    lcd.print(">");
  }
}

void setLevelScreen(){
  lcd.setCursor(0,0);
  lcd.print("Set startLevel:");
  lcd.setCursor(15,0);
  lcd.print(startingLevelValue);
  lcd.setCursor(0,1);
  lcd.print("Press SW-go back");

  valX = analogRead(joyX);

   if (valX > 400 && valX < 600 && valY > 400 && valY < 600) {
    joyMoved = false;
  }
  
  if (!joyMoved && valX < 400) {
    if (startingLevelValue >= 3) {
      startingLevelValue = 1;
    }
    else {
      startingLevelValue++;
    }
    joyMoved = true;
  }
  if (!joyMoved && valX > 600) {
    if (startingLevelValue <= 1) {
      startingLevelValue = 3;
    }
    else {
      startingLevelValue--;
    }
    joyMoved = true;
  }
}

void settingsMenu(){
  lcd.setCursor(1,0);
  lcd.print("StartLevel");
  lcd.setCursor(1,1);
  lcd.print("Contrast");
  lcd.setCursor(11,1);
  lcd.println("Back");
  
  valX = analogRead(joyX);
  valY = analogRead(joyY);
  valSW = digitalRead(joySW);

   if (valX > 400 && valX < 600 && valY > 400 && valY < 600) {
    joyMoved = false;
  }
  
  if (valX < 400 && currentOptionSettings == 'c') {
    currentOptionSettings = 'l';
    joyMoved = true;
  }
  if (valX > 600 && currentOptionSettings == 'l') {
    currentOptionSettings = 'c';
    joyMoved = true;
  }
  if (valY > 600 && currentOptionSettings == 'b') {
    currentOptionSettings = 'c';
    joyMoved = true;
  }
  if (valY <400 && currentOptionSettings == 'c') {
    currentOptionSettings = 'b';
    joyMoved = true;
  }
  if (valSW == LOW && !SWPressed) {
    SWPressed = true;

  if (currentOptionSettings == 'l') {
    settingsScreenToShow = 'l';
  }

  else if (currentOptionSettings == 'b') {
    screenToShow = 'm';
    settingsScreenToShow = 'x'; 
    /* the next time we enter the menu we should not have a screen that maches something 
    in the menu, as default display, we let the player chosse again */
    currentOptionSettings = 'l'; 
    /*the next time we enter this menu, the cursor will stay on the aboutGame 
    option, not on the last option(back) */
  }
  else if (currentOptionSettings == 'c') {
    settingsScreenToShow = 'c';
  }
}
 showCursorForSettings(currentOptionSettings);
}

void contrastScreen() {
  lcd.setCursor(0,0);
  lcd.println("Use the potenti-");
  lcd.setCursor(0,1);
  lcd.println("ometer to adjust");
}

void seeHighScoreScreen(int score) {
  lcd.setCursor(0,0);
  lcd.print("HighScore:");
  lcd.setCursor(10,0);
  lcd.print(score);
  lcd.setCursor(0,1);
  lcd.print("Press SW-go back");
}

// this function lets the player press again the SW button and he gets back to the main menu
void pressAgain() {
  valSW = digitalRead(joySW);
  if (valSW == LOW && !SWPressed) {
     SWPressed = true;
     lcd.clear();
     screenToShow = 'm';
  }
  if (valSW == HIGH) {
   SWPressed = false;
  }
}

/* this function allows the player to play again, reseting all the relevant values(number of lives, 
score, snake length) to their initial value, and it also sets the speed corresponding to the chosen level */
void resetForNewGame(){
  foodX = 5;
  foodY = 2;
  
  isPlaying = true; 
  gameOver = false;
  lives = 3;
  for(int i = 0; i < snakeLength; i++)
  for(int j = 0; j < 2; j++) {
    snake[i][j] = 0;
  }
  snakeLength = 2;
  int headX = 0, headY = 1;
  int endX = 0, endY = 0;
  snake[0][0] = headX;
  snake[0][1] = endX;
  snake[1][0] = headY;
  snake[1][1] = endY;
  level = startingLevelValue;
  char currentDirection = 'N';
  char nextDirection = 'N';
  score=pointsEaten = 0;
  if(startingLevelValue == 1) {
    intervalMillis = 500;
  }
  else if(startingLevelValue == 2){
    intervalMillis = 250;
  }
  else if(startingLevelValue == 3){
    intervalMillis = 200;
  }
  lc.clearDisplay(0);
  lc.setLed(0,foodX, foodY,true);
}

/* a function to show the main menu options, and based on the joystick moves, to call the function for showing the cursor */
void mainMenuScreen() {
  lcd.setCursor(1,0);
  lcd.print("Play");
  lcd.setCursor(7,0);
  lcd.print("Settings"); 
  lcd.setCursor(1,1);
  lcd.print("Highscore"); 
  lcd.setCursor(12,1);
  lcd.print("Info"); 

  valX = analogRead(joyX);
  valY = analogRead(joyY);
  valSW = digitalRead(joySW);

   if (valX > 400 && valX < 600 && valY > 400 && valY < 600) {
    joyMoved = false;
  }
  
  if (valX < 400 && currentOption == 'h') {
    currentOption = 'p';
    joyMoved = true;
  }
  if (valX > 600 && currentOption == 'p') {
    currentOption = 'h';
    joyMoved = true;
  }
  if (valY > 600 && currentOption == 's') {
    currentOption = 'p';
    joyMoved = true;
  }
  if (valY <400 && currentOption == 'p') {
    currentOption = 's';
    joyMoved = true;
  }
   if (valX > 600 && currentOption == 's') {
    currentOption = 'i';
    joyMoved = true;
  }
  if (valX < 400 && currentOption == 'i') {
    currentOption = 's';
    joyMoved = true;
  }
  if (valY < 400 && currentOption == 'h') {
    currentOption = 'i';
    joyMoved = true;
  }
  if (valY > 600 && currentOption == 'i') {
    currentOption = 'h';
    joyMoved = true;
  }

   if (valSW == LOW && !SWPressed) {
    SWPressed = true;

    if (currentOption == 'p') {
      startGameTime = millis();
      screenToShow = 'p';
      isPlaying = true;
      resetForNewGame();
    }
    else if (currentOption == 'h') {
      screenToShow = 'h';
    }
    else if (currentOption == 's') {
      screenToShow = 's';
    }
    else if (currentOption == 'i') {
      screenToShow = 'i';
    }
   }
  
  showCursor(currentOption);
}

void showInfoCursor(int option){
  if(option == 'a') {
    lcd.setCursor(0,0);
    lcd.print(">");
    lcd.setCursor(0,1);
    lcd.print(" ");
    lcd.setCursor(11,0);
    lcd.print(" ");
  }
   if(option == 'g') {
    lcd.setCursor(0,1);
    lcd.print(">");
    lcd.setCursor(0,0);
    lcd.print(" ");
    lcd.setCursor(11,0);
    lcd.print(" ");
  }
   if(option == 'b') {
    lcd.setCursor(11,0);
    lcd.print(">");
    lcd.setCursor(0,1);
    lcd.print(" ");
    lcd.setCursor(0,0);
    lcd.print(" ");
  }
}

void infoMenu(){
  lcd.setCursor(1,0);
  lcd.print("AboutGame");
  lcd.setCursor(12,0);
  lcd.print("Back"); 
  lcd.setCursor(1,1);
  lcd.print("GithubLink"); 

  valX = analogRead(joyX);
  valY = analogRead(joyY);
  valSW = digitalRead(joySW);

   if (valX > 400 && valX < 600 && valY > 400 && valY < 600) {
    joyMoved = false;
  }
  
  if (valX < 400 && currentInfoOption == 'g') {
    currentInfoOption = 'a';
    joyMoved = true;
  }
  if (valX > 600 && currentInfoOption == 'a') {
    currentInfoOption = 'g';
    joyMoved = true;
  }
  if (valY < 400 && currentInfoOption == 'a') {
      currentInfoOption = 'b';
      joyMoved = true;
   }
  if (valY > 600 && currentInfoOption == 'b') {
      currentInfoOption = 'a';
      joyMoved = true;
   }
  if (valSW == LOW && !SWPressed) {
     SWPressed = true;
     
    if (currentInfoOption == 'a') {
      infoScreenToShow = 'a';
     
    }
    else if (currentInfoOption == 'g') {
      infoScreenToShow = 'g';
    }
    else if (currentInfoOption == 'b') {
      infoScreenToShow = 'x';
      screenToShow = 'm';
      currentInfoOption = 'a';
    }
  }
 showInfoCursor(currentInfoOption);
}
 
void displaySettings(){
    valSW = digitalRead(joySW);
    
      if (SWPressed) {
        lcd.clear();
      }
      if (valSW == HIGH) {
         SWPressed = false;
      }
      if(settingsScreenToShow == 'l' && screenToShow == 's') {
        setLevelScreen();
        pressAgain();
      }
      else if(settingsScreenToShow == 'c' && screenToShow == 's') {
        contrastScreen();
        pressAgain();
      }
  }
  
/* this function lets the player press again the SW button and he gets back to the previous menu, the settings menu */
void pressAgainForSettings() {
    valSW = digitalRead(joySW);
  if (valSW == LOW && !SWPressed) {
     SWPressed = true;
     lcd.clear();
     screenToShow = 's';
     settingsScreenToShow = 'x';
  }
  if (valSW == HIGH) {
   SWPressed = false;
  }
}

/* this function lets the player press again the SW button and he gets back to the previous menu, the info menu */
void pressAgainForInfo() {
    valSW = digitalRead(joySW);
    if (valSW == LOW && !SWPressed) {
       SWPressed = true;
       lcd.clear();
       screenToShow = 'i';
       infoScreenToShow = 'x';
    }
    if (valSW == HIGH) {
     SWPressed = false;
    }
}

void aboutGameScreen(){
  lcd.setCursor(0,0);
  lcd.print("Snake-by Claudia");
  lcd.setCursor(0,1);
  lcd.print("Press SW-back"); 
}

void githubScreen() {
  lcd.setCursor(0,0);
  lcd.print("github.com/Claud");
  lcd.setCursor(0,1);
  lcd.print("iaursu"); 
}

void loop() {

  valSW = digitalRead(joySW);
 
  if (SWPressed) {
    lcd.clear();
  }
  
  if (valSW == HIGH) {
     SWPressed = false;
  }
  
  if(screenToShow == 'm') {
     imageDisplay(notPlayingImage);
     mainMenuScreen();
  }

  else if(screenToShow == 's') {
   if(settingsScreenToShow == 'c') {
     contrastScreen();
     pressAgainForSettings();
   }
   else if(settingsScreenToShow == 'l') {
     setLevelScreen();
     Serial.println(startingLevelValue);
     pressAgainForSettings();
   }
   else {
     settingsMenu();
   }
  }
  
   else if(screenToShow == 'i') {
   if(infoScreenToShow == 'a') {
      aboutGameScreen();
      pressAgainForInfo();
    }
    else if(infoScreenToShow == 'g') {
      githubScreen();
       pressAgainForInfo();
    }
     else {
     infoMenu();
   }
  }
  else if(screenToShow == 'h') {
    seeHighScoreScreen(highScore);
    pressAgain();
  }
  
  else if(screenToShow == 'p') {   
     if(isPlaying) {
       playGame();
       playingScreen();
     }
     if(gameOver) {
      if(score != highScore) {
        gameOverScreen(score);
        pressAgain();
     }
     else {
        highScoreScreen(score);
        pressAgain();
     }
      
    }
  }
}
