#include <EEPROM.h>
#include <LiquidCrystal_I2C.h>
#include <ThreeWire.h>
#include <RtcDS1302.h>

LiquidCrystal_I2C lcd(0x27, 16, 2);  // set the LCD address to 0x27 for a 16 chars and 2 line display
ThreeWire myWire(5, 3, 7); //(DAT, CLK, RST)
RtcDS1302<ThreeWire> Rtc(myWire);

// Define EEPROM addresses for storing alarm settings
int eepromAddressHour = 0;
int eepromAddressMinute = 1;

// Variables to store alarm time
int alarmHour = 0;
int alarmMinute = 0;

int joystickMidValue = 10;
int joystickXAxis = 0;
int joystickYAxis = 0;
int joystickButton = 0;

//bool joystickPressed = false;
//bool scrollScreen = false;

int streak = 0;

// custom design
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

void setup() {
  // LCD setup
  lcd.init();
  lcd.backlight();
  lcd.clear();
  lcd.createChar(0, clockDesign);

  // RTC setup
  Rtc.Begin();

  // get current time
  // only run once
  // RtcDateTime currentTime= RtcDateTime(__DATE__, __TIME__);
  // Rtc.SetDateTime(currentTime);

  // Read alarm from EEPROM
  alarmHour = EEPROM.read(eepromAddressHour); // or just 0
  alarmMinute = EEPROM.read(eepromAddressMinute); // or just 1
  //Check if the numbers that you read are valid. (Hours:0-23 and Minutes: 0-59)
  if (alarmHour>23){
    alarmHour=0;
  }
  if (alarmMinute>59){
    alarmMinute=0;
  }

  // joystick inputs
  pinMode(A0, INPUT);
  pinMode(A1, INPUT);
  pinMode(A2, INPUT);

  // light LED
  digitalWrite(12, HIGH);
  digitalWrite(10, HIGH);
  digitalWrite(8, HIGH);

  // prob A# will change
  joystickXAxis = analogRead(A2); // initial x position
  joystickYAxis = analogRead(A1); // initial y position
  joystickButton = analogRead(A0);
}

void loop() {
  RtcDateTime now = Rtc.GetDateTime();
  
  int xAxis = analogRead(A2); // current x position
  int yAxis = analogRead(A1); // current y position
  int joystickButton = analogRead(A0); // current value of button

  // Check if the current time matches the set alarm
  if (now.Hour() == alarmHour && now.Minute() == alarmMinute) {
    // Perform your action here
    performAction();
    delay(60000); // Delay for a minute to avoid continuous actions
  }

  // Check if the joystick button is pressed
  if (joystickButton == 0) {
    // set alarm
    setAlarm();
    delay(500);
  }

  defaultScreen();

  // print current date + time
  lcd.setCursor(0, 0);  // 1st row
  lcd.print(now.Day());
  lcd.print("-");
  lcd.print(now.Month());
  lcd.print("-");
  lcd.print(now.Year());
  lcd.setCursor(12, 0); // try -5,0 as well
  lcd.print(now.Hour());
  lcd.print(":");
  lcd.print(now.Minute());

  // print alarm + design + streak
  lcd.setCursor(0,1);   // 2nd row
  // print alarm variable
  lcd.write(0);
  lcd.setCursor(16,1);
  lcd.print(streak);

  // Check for upward movement
  if (yAxis > joystickYAxis + joystickMidValue) {
    // Joystick moved upward
    // Your code here
  }

  // Check for downward movement
  if (yAxis < joystickYAxis - joystickMidValue) {
    // Joystick moved downward
    // Your code here
  }

  // Check for rightward movement
  if (xAxis > joystickXAxis + joystickMidValue) {
    // Joystick moved right
    // Your code here
  }

  // Check for leftward movement
  if (xAxis < joystickXAxis - joystickMidValue) {
    // Joystick moved left
    // Your code here
  }

  delay(500);
}

void setAlarm() {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Set Alarm:");

  // Loop for setting the hour
  while (true) {
    lcd.setCursor(0, 1);
    lcd.print(alarmHour);
    lcd.print(":");
    lcd.print(alarmMinute);

    int yAxis = analogRead(A1);

    if (yAxis > joystickYAxis + joystickMidValue) {
      // Move up to increase the hour
      incrementHour();
    }

    if (yAxis < joystickYAxis - joystickMidValue) {
      // Move down to decrease the hour
      decrementHour();
    }

    // Check if the joystick button is pressed to confirm the hour
    if (joystickButton == 0) {
      delay(500); // Debounce delay
      break;
    }
  }

  // Loop for setting the minute
  while (true) {
    lcd.setCursor(0, 1);
    lcd.print(alarmHour);
    lcd.print(":");
    lcd.print(alarmMinute);

    int yAxis = analogRead(A1);

    if (yAxis > joystickYAxis + joystickMidValue) {
      // Move up to increase the minute
      incrementMinute();
    }

    if (yAxis < joystickYAxis - joystickMidValue) {
      // Move down to decrease the minute
      decrementMinute();
    }

    // Check if the joystick button is pressed to confirm the minute
    if (joystickButton == 0) {
      delay(500); // Debounce delay
      break;
    }
  }

  // Exit alarm-setting mode
  lcd.clear();

  // Save alarm settings to EEPROM
  EEPROM.write(eepromAddressHour, alarmHour);
  EEPROM.write(eepromAddressMinute, alarmMinute);
}

void incrementHour() {
  alarmHour = (alarmHour + 1) % 24;
  delay(200); // Delay to avoid rapid changes
}

void decrementHour() {
  alarmHour = (alarmHour - 1 + 24) % 24;
  delay(200); // Delay to avoid rapid changes
}

void incrementMinute() {
  alarmMinute = (alarmMinute + 1) % 60;
  delay(200); // Delay to avoid rapid changes
}

void decrementMinute() {
  alarmMinute = (alarmMinute - 1 + 60) % 60;
  delay(200); // Delay to avoid rapid changes
}

void performAction() {
  // This is where you define the action to be performed when the alarm goes off
  // For example, you can turn on a buzzer, display a message, etc.
  // Add your code here
}

void defaultScreen() {
  
}