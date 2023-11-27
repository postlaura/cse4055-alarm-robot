#include <LiquidCrystal_I2C.h>
LiquidCrystal_I2C lcd(0x27, 16, 2);

// pins
const int lightSensorPin = A3;
const int buzzerPin = 11;
const int buttonBlue = 5;
const int buttonGreen = 6;
const int buttonRed = 7;
const int blueLED = 8;
const int greenLED = 9;
const int redLED = 10;

// joystick variables
int joystickButton = 0;
const int joystickButtonPin = A0;

// led variables
const int LEDTab[3] = {blueLED, greenLED, redLED};
int LEDSequence[4];
int userInput[4];

// alarm variables
bool alarmTime = true;
bool sequenceFinished = false;
unsigned long countdown = 10000;
unsigned long screenMillis = 0;

// alarm sound
const int NOTE_C4  = 523;
const int NOTE_CS4 = 554;
const int NOTE_D4  = 587;
const int NOTE_DS4 = 622;
const int NOTE_E4  = 659;
const int NOTE_F4  = 698;
const int NOTE_FS4 = 740;
const int NOTE_G4  = 784;
const int NOTE_GS4 = 831;
const int NOTE_A4  = 880;
const int NOTE_AS4 = 932;
const int NOTE_B4  = 987;
const int* alarmSound[] = {NOTE_C4, NOTE_CS4,
NOTE_D4, NOTE_DS4, NOTE_E4, NOTE_F4, NOTE_FS4,
NOTE_G4, NOTE_GS4, NOTE_A4, NOTE_AS4, NOTE_B4};

// custom designs
byte arrowDesign[8] = {
  0b00000,
	0b00100,
	0b00100,
	0b00100,
	0b10101,
	0b01110,
	0b00100,
	0b00000
};
byte sadFaceDesign[8] = {
  0b00000,
  0b00000,
  0b01010,
  0b00000,
  0b01110,
  0b10001,
  0b00000,
  0b00000
};


// function declaration
void alarm();
void alarmScreen();
void generateSequence();
void matchLED();
void notMatchLED();



void setup() {
  // in case of debuggin
  Serial.begin(9600);
  randomSeed(analogRead(0));

  // lcd set up
  lcd.init();
  lcd.backlight();
  lcd.clear();
  lcd.createChar(2, arrowDesign);
  lcd.createChar(3, sadFaceDesign);

  // pin modes
  pinMode(blueLED,OUTPUT);
  pinMode(greenLED,OUTPUT);
  pinMode(redLED,OUTPUT);
  pinMode(buzzerPin, OUTPUT);
  pinMode(joystickButtonPin, INPUT);
  pinMode(buttonBlue, INPUT);
  pinMode(buttonGreen, INPUT);
  pinMode(buttonRed, INPUT);
  pinMode(lightSensorPin, INPUT);
}



// main loop
void loop() {

  if (alarmTime) {
    generateSequence();
    sequenceFinished = false;
    alarm();
  }

  lcd.clear();
}


void alarm() {

  // buzzer on
  tone(buzzerPin, alarmSound[random(0, 12)]);

  // message to user
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("WAKE UP!");
  

  // wait for user to press joystick
  while (analogRead(joystickButtonPin) != 0) {
    delay(50);
  }
  // user pressed the button, display LED sequence
  delay(200);

  // loop that displays LED sequence and check for user input
  // will loop until the user inputs the correct sequence
  while (!sequenceFinished) {

    // display random sequence
    for (int i = 0; i < sizeof(LEDSequence) / sizeof(LEDSequence[0]); i++){
      alarmScreen();  // update countdown

      digitalWrite(LEDSequence[i], HIGH);
      delay(700);
      digitalWrite(LEDSequence[i], LOW);
      delay(700);
    }
    delay(100);

    // check user input
    int i = 0;  // auxiliary variable
    while (i < 4) {
      alarmScreen();  // update countdown

      if(debounceButton(buttonBlue) == HIGH){
        // record user input
        userInput[i] = blueLED;
        // turn on light
        digitalWrite(blueLED, HIGH);
        delay(250);
        digitalWrite(blueLED, LOW);
        delay(250);
        i++;  // update auxiliary variable
      }
      if(debounceButton(buttonGreen) == HIGH){
        // record user input
        userInput[i] = greenLED;
        // turn on light
        digitalWrite(greenLED, HIGH);
        delay(250);
        digitalWrite(greenLED, LOW);
        delay(250);
        i++;  // update auxiliary variable
      }
      if (debounceButton(buttonRed) == HIGH){
        // record user input
        userInput[i] = redLED;
        // turn on light
        digitalWrite(redLED, HIGH);
        delay(250);
        digitalWrite(redLED, LOW);
        delay(250);
        i++;  // update auxiliary variable
      }
    }

    // check if user input is the correct sequence
    bool match = true;
    for (int i = 0; i < sizeof(LEDSequence)/sizeof(LEDSequence[0]); i++){
      if(LEDSequence[i] != userInput[i]){
        match = false;
        break;
      }
    }

    if(match){

      // matchLED sequence
      matchLED();

      sequenceFinished = true;
      alarmScreen();
      delay(500);
    } else {

      // notMatchLED sequence
      notMatchLED();
    }
  }
}



void alarmScreen(){

  // user finished sequence
  if (sequenceFinished){

    // wait for user to turn on the lights
    while (analogRead(lightSensorPin) < 700){
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Turn on");
      lcd.setCursor(0, 1);
      lcd.print("the lights!");
      delay(100);
    }

    // correct sequence + lightValue -> user woke up
    // buzzer off
    noTone(buzzerPin);
    // display message
    lcd.clear();
    lcd.print("Good morning!");
    // update alarm variables
    sequenceFinished = true;
    alarmTime = false;
    delay(5000);
  }
  // user didn't input the correct sequence
  else {

    // display countdown
    unsigned long currentMillis = millis();
    if (currentMillis - screenMillis >= 1000 && countdown > 0 && countdown != 1) {
      screenMillis = currentMillis;

      // change buzzer tone
      tone(buzzerPin, alarmSound[random(0, 12)]);
      // display minutes and seconds left
      lcd.setCursor(0, 1);
      int minutes = countdown / 60000;
      int seconds = (countdown % 60000) / 1000;
      lcd.print(minutes);
      lcd.print(":");
      if (seconds < 10) {
        lcd.print("0");
      }
      lcd.print(seconds);
      lcd.print(" ");
      lcd.write(2);
      countdown -= 1000;  // -1s

    // countdown 0
    } else if (countdown == 0) {
      countdown = 1;

      // display message
      lcd.clear();
      lcd.print("Today is marked");
      lcd.setCursor(0, 1);
      lcd.print("with an X... ");
      lcd.write(3);
      delay(5000);

      // continue until the user inputs the correct sequence
      alarm();
    }
  }
}

// function to generate a random sequence
void generateSequence() {
  for (int i = 0; i < 4; i++) {
    LEDSequence[i] = LEDTab[random(0, 3)];
  }
}

// auxiliary function to debounce a button
int debounceButton(int buttonPin) {
  int currentState = digitalRead(buttonPin);
  delay(5);
  int newState = digitalRead(buttonPin);
  return (currentState == newState) ? currentState : -1;
}

// match sequence LED
void matchLED() {
  // 3 HIGH/LOW SHORT
  digitalWrite(blueLED, HIGH);
  digitalWrite(greenLED, HIGH);
  digitalWrite(redLED, HIGH);
  delay(300);
  digitalWrite(blueLED, LOW);
  digitalWrite(greenLED, LOW);
  digitalWrite(redLED, LOW);
  delay(300);
  digitalWrite(blueLED, HIGH);
  digitalWrite(greenLED, HIGH);
  digitalWrite(redLED, HIGH);
  delay(300);
  digitalWrite(blueLED, LOW);
  digitalWrite(greenLED, LOW);
  digitalWrite(redLED, LOW);
  delay(300);
  digitalWrite(blueLED, HIGH);
  digitalWrite(greenLED, HIGH);
  digitalWrite(redLED, HIGH);
  delay(500);
  digitalWrite(blueLED, LOW);
  digitalWrite(greenLED, LOW);
  digitalWrite(redLED, LOW);
  delay(500);
}

// not match sequence LED
void notMatchLED() {
  // 1 HIGH/LOW LONG
  digitalWrite(blueLED, HIGH);
  digitalWrite(greenLED, HIGH);
  digitalWrite(redLED, HIGH);
  delay(500);
  digitalWrite(blueLED, LOW);
  digitalWrite(greenLED, LOW);
  digitalWrite(redLED, LOW);
  delay(500);
}