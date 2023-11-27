#include <EEPROM.h>
#include <LiquidCrystal_I2C.h>
#include <ThreeWire.h>
#include <RtcDS1302.h>

// set up
LiquidCrystal_I2C lcd(0x27, 16, 2);  // set the LCD address to 0x27 for a 16 chars and 2 line display
ThreeWire myWire(3, 2, 4); //(DAT, CLK, RST)
RtcDS1302<ThreeWire> Rtc(myWire);

// pins
const int lightSensorPin = A3;
const int buzzerPin = 11;
const int buttonBlue = 5;
const int buttonGreen = 6;
const int buttonRed = 7;
const int blueLED = 8;
const int greenLED = 9;
const int redLED = 10;
const int joystickXPin = A2;
const int joystickYPin = A1;
const int joystickButtonPin = A0;

// alarm variables
int alarmHour = 0;
int alarmMinute = 0;
bool sequenceFinished = false;
unsigned long countdown = 0;
unsigned long screenMillis = 0;

// joystick variables
int joystickMidValue = 20;
int joystickXAxis = 0;
int joystickYAxis = 0;
int joystickButton = 0;

// led variables
const int LEDTab[3] = {blueLED, greenLED, redLED};
const int LEDSequenceLength = 4;
int LEDSequence[LEDSequenceLength];
int userInput[LEDSequenceLength];

// user variables
int streak = 0;
int statisticList[7] = {4, 4, 4, 4, 4, 4, 4};
int currentDay = 0;

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
byte clockDesign[8] = {
  0b00000,
  0b00100,
  0b01110,
  0b01110,
  0b11111,
  0b00000,
  0b00100,
  0b00000,
};
byte streakDesign[8] = {
  0b00010,
  0b00100,
  0b01000,
  0b11111,
  0b00010,
  0b00100,
  0b01000,
  0b00000
};
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
byte ODesign[8] = {
  0b00000,
  0b01110,
  0b10001,
  0b10001,
  0b10001,
  0b10001,
  0b01110,
  0b00000
};
byte checkmarkDesign[8] = {
  0b00000,
  0b00000,
  0b00001,
  0b00010,
  0b10100,
  0b01000,
  0b00000,
  0b00000
};
byte XDesign[8] = {
  0b00000,
  0b00000,
  0b10001,
  0b01010,
  0b00100,
  0b01010,
  0b10001,
  0b00000
};

// function declarations
// alarm functions
void alarm(RtcDateTime now);
void generateSequence();
void debounceButton();
void matchLED();
void notMatchLED();
void setAlarm();
void incrementHour();
void decrementHour();
void incrementMinute();
void decrementMinute();
// screens
void alarmScreen(RtcDateTime now);
void defaultScreen(RtcDateTime now);
void statisticScreen();



void setup() {
  // in case of debuggin
  Serial.begin(9600);
  randomSeed(analogRead(0));

  // LCD setup
  lcd.init();
  lcd.backlight();
  lcd.clear();
  lcd.createChar(0, clockDesign);
  lcd.createChar(1, streakDesign);
  lcd.createChar(2, arrowDesign);
  lcd.createChar(3, sadFaceDesign);
  lcd.createChar(4, ODesign);
  lcd.createChar(5, checkmarkDesign);
  lcd.createChar(6, XDesign);

  // RTC setup
  Rtc.Begin();

  // get current time
  // only run once
  RtcDateTime currentTime= RtcDateTime(__DATE__, __TIME__);
  Rtc.SetDateTime(currentTime);

  // read info from EEPROM
  alarmHour = EEPROM.read(0);
  alarmMinute = EEPROM.read(1);
  streak = EEPROM.read(2);
  for (int i = 0; i < 7; i++) {
  statisticList[i] = EEPROM.read(3 + i);
  }


  // pin modes
  pinMode(blueLED,OUTPUT);
  pinMode(greenLED,OUTPUT);
  pinMode(redLED,OUTPUT);
  pinMode(buzzerPin, OUTPUT);
  pinMode(buttonBlue, INPUT);
  pinMode(buttonGreen, INPUT);
  pinMode(buttonRed, INPUT);
  pinMode(lightSensorPin, INPUT);
  pinMode(joystickXPin, INPUT);
  pinMode(joystickYPin, INPUT);
  pinMode(joystickButtonPin, INPUT);

  //joystick values
  joystickXAxis = analogRead(joystickXPin); // initial x position
  joystickYAxis = analogRead(joystickYPin); // initial y position
}



void loop() {
  // get current time
  RtcDateTime now = Rtc.GetDateTime();

  // check if it's alarm time
  if (now.Hour() == alarmHour && now.Minute() == alarmMinute && now.Second() == 0) {
    generateSequence();
    sequenceFinished = false;
    alarm(now);
  } else {
    Serial.println("Calling default");
    defaultScreen(now);
  }

  delay(500);
}



// alarm function -> display LED sequence + check user input
void alarm(RtcDateTime now) {

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
      Serial.println("Called by 1");
      alarmScreen(now);  // update countdown

      digitalWrite(LEDSequence[i], HIGH);
      delay(700);
      digitalWrite(LEDSequence[i], LOW);
      delay(700);
    }
    delay(100);

    // check user input
    int i = 0;  // auxiliary variable
    while (i < 4) {
      if (sequenceFinished) {
        break;
      }
      Serial.println("Called by 2");
      alarmScreen(now);  // update countdown

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
      break;
    } else {

      // notMatchLED sequence
      notMatchLED();
    }
  }
  Serial.println("Called by 4");
  alarmScreen(now);
}



// setAlarm function -> user sets the alarm and store it in EEPROM
void setAlarm() {

  alarmHour = 0;
  alarmMinute = 0;

  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Set alarm: Hour");
  delay(500); // visual delay to user

  // setting the hour
  while (true) {
    lcd.setCursor(0, 1);
    if (alarmHour < 10) {
      lcd.print("0");
    }
    lcd.print(alarmHour);
    lcd.print(":");
    if (alarmMinute < 10) {
      lcd.print("0");
    }
    lcd.print(alarmMinute);

    int yAxis = analogRead(joystickYPin); // current y position
    int joystickButton = analogRead(joystickButtonPin); // current value of button

    // joystick moves up, increment hour
    if (yAxis < joystickYAxis - joystickMidValue) {
      incrementHour();
    }

    // joystick moves down, decrement hour
    if (yAxis > joystickYAxis + joystickMidValue) {
      // Move down to decrease the hour
      decrementHour();
    }

    // joystick pressed, confirm hour
    if (joystickButton == 0) {
      delay(100);
      break;
    }
  }

  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Set alarm: Min");
  delay(500); // visual delay to user
  
  // setting the minute
  while (true) {
    lcd.setCursor(0, 1);
    if (alarmHour < 10) {
      lcd.print("0");
    }
    lcd.print(alarmHour);
    lcd.print(":");
    if (alarmMinute < 10) {
      lcd.print("0");
    }
    lcd.print(alarmMinute);

    int yAxis = analogRead(joystickYPin); // current y position
    int joystickButton = analogRead(joystickButtonPin); // current value of button

    // joystick moves up, increment minute
    if (yAxis < joystickYAxis - joystickMidValue) {
      incrementMinute();
    }
    // joystick moves down, decrement minute
    if (yAxis > joystickYAxis + joystickMidValue) {
      decrementMinute();
    }

    // joystick pressed, confirm minute
    if (joystickButton == 0) {
      delay(100);
      break;
    }
    delay(100);
  }

  // save alarm
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Saving alarm...");
  delay(800); // visual delay to user
  EEPROM.write(0, alarmHour);
  EEPROM.write(1, alarmMinute);

  // set countdown value (60 seconds)
  countdown = 60000;
}

void incrementHour() {
  alarmHour = (alarmHour + 1) % 24;
  delay(200);
}
void decrementHour() {
  alarmHour = (alarmHour - 1 + 24) % 24;
  delay(200);
}
void incrementMinute() {
  alarmMinute = (alarmMinute + 1) % 60;
  delay(200);
}
void decrementMinute() {
  alarmMinute = (alarmMinute - 1 + 60) % 60;
  delay(200);
}



// alarmScreen -> display countdown and messages to user
void alarmScreen(RtcDateTime now){

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

    // user woke up before countdown reached 0
    if (countdown > 1) {
      // update streak
      streak += 1;
      // update statistic
      currentDay = now.DayOfWeek();
      statisticList[currentDay] = 5;
    }

    // save variables in EEPROM
    EEPROM.write(2, streak);
    for (int i = 0; i < 7; i++) {
      Serial.println(statisticList[i]);
      EEPROM.write(3 + i, statisticList[i]);
    }

    delay(5000);
    Serial.println("End of delay");
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
      // break streak and statistic
      streak = 0;
      currentDay = now.DayOfWeek();
      statisticList[currentDay] = 6;

      // display message
      lcd.clear();
      lcd.print("Today is marked");
      lcd.setCursor(0, 1);
      lcd.print("with an X... ");
      lcd.write(3);
      delay(5000);

      // continue until the user inputs the correct sequence
      alarm(now);
    }
  }
  Serial.println("Finish alarmScreen");
}



// defaultScreen -> display current date, time, streak and alarm
void defaultScreen(RtcDateTime now) {

  // read values
  int xAxis = analogRead(joystickXPin); // current x position
  int yAxis = analogRead(joystickYPin); // current y position
  int joystickButton = analogRead(joystickButtonPin); // current value of button

  //check if user presses joystick
  if (joystickButton == 0) {
    // joystick was pressed
    // set alarm
    setAlarm();
    delay(500);
  
  // check downward movement
  } else if (yAxis > joystickYAxis + joystickMidValue){
    // joystick moved downward
    statisticScreen();

  } else {
    lcd.clear();
    // print current date + time
    lcd.setCursor(0, 0);  // 1st row
    lcd.print(now.Day());
    lcd.print("-");
    lcd.print(now.Month());
    lcd.print("-");
    lcd.print(now.Year());
    lcd.setCursor(11, 0);
    if (now.Hour() < 10) {
      lcd.print("0");
    }
    lcd.print(now.Hour());
    lcd.print(":");
    if (now.Minute() < 10) {
      lcd.print("0");
    }
    lcd.print(now.Minute());

    // print alarm + design + streak
    lcd.setCursor(0, 1);   // 2nd row
    if (alarmHour < 10) {
      lcd.print("0");
    }
    lcd.print(alarmHour);
    lcd.print(":");
    if (alarmMinute < 10) {
      lcd.print("0");
    }
    lcd.print(alarmMinute);
    lcd.write(0);
    lcd.setCursor(13, 1);
    lcd.write(1);
    if (streak < 10){
      lcd.print("0");
    }
    lcd.print(streak);
  }
}



// statisticScreen -> display statistics
void statisticScreen(){
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("S M T W T F S");
  lcd.setCursor(0, 1);
  for (int i = 0; i < 7; i ++) {
    lcd.write(statisticList[i]);
    lcd.print(" ");
  }

  while (!(analogRead(joystickYPin) < joystickYAxis - joystickMidValue)) {  // checks if joystick is moved upwards
    // Joystick did not move upwards
    delay(10);
  }
  // joystick moved upwards, exit statisticScreen
}



// function to generate a random sequence
void generateSequence() {
  for (int i = 0; i < LEDSequenceLength; i++) {
    LEDSequence[i] = LEDTab[random(0, LEDSequenceLength - 1)];
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













  //// Check for upward movement
  //if (yAxis < joystickYAxis - joystickMidValue) {
  //  // Joystick moved upward
  //  // Your code here
  //}
//
  //// Check for downward movement
  //if (yAxis > joystickYAxis + joystickMidValue) {
  //  // Joystick moved downward
  //  // Your code here
  //}
//
  //// Check for rightward movement
  //if (xAxis > joystickXAxis + joystickMidValue) {
  //  // Joystick moved right
  //  // Your code here
  //}
//
  //// Check for leftward movement
  //if (xAxis < joystickXAxis - joystickMidValue) {
  //  // Joystick moved left
  //  // Your code here
  //}