const int passivePin = A0; 
const int activePin = 8;
const int buttonPin = 2;
const int threshold = 7;

int sensorValue = 0;
int buttonState = 1;
int readingButton;
int songPlaying = 0;
int lastButtonState = 1;
int knockTime;  //the time when the knock was detected

unsigned long lastDebounceTime = 0;
unsigned long debounceDelay = 50;


void setup() {

  pinMode(passivePin, INPUT);
  pinMode(activePin, OUTPUT);
  pinMode(buttonPin, INPUT_PULLUP);
  Serial.begin(9600);
}

void loop() {
  sensorValue=analogRead(passivePin);
  if(sensorValue != 0)
  Serial.println(sensorValue);

  if(sensorValue >= threshold && songPlaying == 0) {
    Serial.println("Knock");
    knockTime = millis();
    songPlaying = 1;
  }
  
  if(millis()-knockTime >= 5000 && songPlaying == 1) 
    tone(activePin,500);
   
  readingButton = digitalRead(buttonPin);
  if(readingButton != lastButtonState) {
    lastDebounceTime = millis();
  }

  if( (millis() - lastDebounceTime) > debounceDelay) {
    if(readingButton != buttonState) {
      buttonState = readingButton;
    }
    if(buttonState == 0 && songPlaying == 1) {
      noTone(activePin);
      Serial.println("Music stopped");
      songPlaying = 0;
    }
   
  }
  lastButtonState = readingButton;
  
}
