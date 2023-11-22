#include <EEPROM.h>
#include <LiquidCrystal_I2C.h>
#include <ThreeWire.h>
#include <RtcDS1302.h>

// set up
LiquidCrystal_I2C lcd(0x27, 16, 2);  // set the LCD address to 0x27 for a 16 chars and 2 line display
ThreeWire myWire(3, 2, 4); //(DAT, CLK, RST)
RtcDS1302<ThreeWire> Rtc(myWire);


// alarm variables
int eepromAddressHour = 0;
int eepromAddressMinute = 1;
int alarmHour = 0;
int alarmMinute = 0;
unsigned long previousMillis = 0;

// joystick variables
int joystickMidValue = 20;
int joystickXAxis = 0;
int joystickYAxis = 0;
int joystickButton = 0;

// user variables
int streak = 0;
int countdown = 0;
int sequenceFinished = 0;

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
  0b10101,
  0b01110,
  0b00100,
  0b00000,
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


void setAlarm();
void incrementHour();
void decrementHour();
void incrementMinute();
void decrementMinute();
void checkButtonSequence();
void alarmScreen(unsigned long currentMillis, int lightValue, int sequenceFinished, RtcDateTime now);
void defaultScreen(RtcDateTime now);
void statisticScreen();
void alarm(RtcDateTime now);



void setup() {
  Serial.begin(9600);

  // LCD setup
  lcd.init();
  lcd.backlight();
  lcd.clear();
  lcd.createChar(0, clockDesign);
  lcd.createChar(1, streakDesign);
  lcd.createChar(2, arrowDesign);
  lcd.createChar(3, sadFaceDesign);

  // RTC setup
  Rtc.Begin();

  // get current time
  // only run once
  // RtcDateTime currentTime= RtcDateTime(__DATE__, __TIME__);
  // Rtc.SetDateTime(currentTime);

  // Read alarm from EEPROM
  alarmHour = EEPROM.read(0);
  alarmMinute = EEPROM.read(1);

  // joystick inputs
  pinMode(A0, INPUT);
  pinMode(A1, INPUT);
  pinMode(A2, INPUT);

  // light LED
  //digitalWrite(10, HIGH); // red LED
  //digitalWrite(9, HIGH);  // green LED
  //digitalWrite(8, HIGH);  // blue LED

  // joystick values
  joystickXAxis = analogRead(A2); // initial x position
  joystickYAxis = analogRead(A1); // initial y positiona0
  joystickButton = analogRead(A0);
}



void loop() {
  // get current time
  RtcDateTime now = Rtc.GetDateTime();

  // check if it's alarm time
  if (now.Hour() == alarmHour && now.Minute() == alarmMinute) {
    alarm(now);
  } else {
    defaultScreen(now);
  }

  delay(500);
}



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

    int yAxis = analogRead(A1);          // current y position
    int joystickButton = analogRead(A0); // current value of button

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

    int yAxis = analogRead(A1);          // current y position
    int joystickButton = analogRead(A0); // current value of button

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

  // set countdown value (30 seconds)
  countdown = 30000;
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











void checkButtonSequence() {
  // check that user input is correct
}

// SET UP ALARM SCREEN
void alarmScreen(unsigned long currentMillis, int lightValue, int sequenceFinished, RtcDateTime now) {
  if (currentMillis - previousMillis >= 1000) {
    lcd.clear();
    lcd.setCursor(0, 0);

    // print current time
    lcd.print(now.Hour());
    lcd.print(":");
    lcd.print(now.Minute());

    // print countdown
    lcd.setCursor(0, 1);
    lcd.print(countdown / 1000);
    lcd.print("s");
    lcd.write(1);

    // print light sensor value
    lcd.setCursor(10,0);
    lcd.print("Light sensor: ");
    lcd.print(lightValue);

    previousMillis = currentMillis;
    countdown -= 1000;

    // light sensor reads high value + sequenceFinished -> user woke up, input the correct sequence and turned on the lights
    if (lightValue >= 0 && sequenceFinished <= 0) {
      // congrat user
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Good morning!");
      delay(50000);

      // back to defaultScreen
      //defaultScreen();
      // creo que mejor no poner nada y el loop comienza de nuevo

    // countdown reached 0 before user woke up
    } else if (countdown <= 0){
      // user didn't wake up :(
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("Countdown reached 0!");
        lcd.setCursor(0, 1);
        lcd.write(2);
    }
  }
}



// SET UP OF DEFAULT SCREEN
void defaultScreen(RtcDateTime now) {

  // read values
  int xAxis = analogRead(A2); // current x position
  int yAxis = analogRead(A1); // current y position
  int joystickButton = analogRead(A0); // current value of button

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
    lcd.print(now.Hour());
    lcd.print(":");
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


// SET UP STATISTIC SCREEN
void statisticScreen(){
  lcd.clear();
  lcd.print("Statistic Screen");

  while (!(analogRead(A1) < joystickYAxis - joystickMidValue)) {  // checks if joystick is moved upwards
    // Joystick did not move upwards
    delay(10);
  }
  // joystick moved upwards, exit statisticScreen
}



// alarm code
void alarm(RtcDateTime now) {

  // generate sequence

  // use millis to perform multiple tasks simultaneously
  unsigned long currentMillis = millis();

  // three tasks
  // 1. Check user input
  checkButtonSequence();
  // 2. Read light sensor value
  int lightValue = analogRead(A5);
  // 3. Alarm screen
  alarmScreen(currentMillis, lightValue, sequenceFinished, now);
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