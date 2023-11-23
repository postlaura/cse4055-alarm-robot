#include <LiquidCrystal_I2C.h>
LiquidCrystal_I2C lcd(0x27, 16, 2);

// buzzer pin
const int lightSensorPin = A3;
// button pin
const int buttonBlue = 5;
const int buttonGreen = 6;
const int buttonRed = 7;

// LED pin
const int blueLED = 8;
const int greenLED = 9;
const int redLED = 10;

// joystick variables
int joystickButton = 0;
const int joystickButtonPin = A0;

int const led_tab[3] = {blueLED, greenLED, redLED};

int sequence[4];
int pattern[4];

bool alarmTime = true;
bool sequenceFinished = false;
int countdown = 30000;

unsigned long screenMillis = 0;


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
void generateSequence();
void alarm();
void alarmScreen(bool sequenceFinished);
int debounceButton(int buttonPin) {
  int currentState = digitalRead(buttonPin);
  delay(10);
  int newState = digitalRead(buttonPin);
  return (currentState == newState) ? currentState : -1;
}
void generateSequence() {
  for (int i = 0; i < 4; i++) {
    sequence[i] = led_tab[random(0, 3)];
  }
}


void setup() {
  Serial.begin(9600);
  randomSeed(analogRead(0));

  lcd.init();
  lcd.backlight();
  lcd.clear();
  lcd.createChar(2, arrowDesign);
  lcd.createChar(3, sadFaceDesign);

  pinMode(blueLED,OUTPUT);
  pinMode(greenLED,OUTPUT);
  pinMode(redLED,OUTPUT);

  pinMode(joystickButtonPin, INPUT);

  pinMode(buttonBlue, INPUT);
  pinMode(buttonGreen, INPUT);
  pinMode(buttonRed, INPUT);

  // joystick values
  joystickButton = analogRead(joystickButtonPin);
}



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
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("WAKE UP!");

  // TESTING PURPOSES BECAUSE THE BUTTONS ARE SHIT
  for (int i = 0; i < 4; i++) {
    sequence[i] = led_tab[0];
  }
  

  while (analogRead(joystickButtonPin) != 0) {
    // user did not press button, don't display LED sequence yet
    //Serial.println("Blue button");
    //Serial.println(debounceButton(buttonBlue));
    //digitalWrite(buttonBlue, LOW);
    //Serial.println("Green button");
    //Serial.println(debounceButton(buttonGreen));
    //digitalWrite(buttonGreen, LOW);
    //Serial.println("Red button");
    //Serial.println(debounceButton(buttonRed));
    //digitalWrite(buttonRed, LOW);
    delay(100);
  } // user pressed the button, display LED sequence
  delay(200);

  while (!sequenceFinished) {

    if (countdown <= 0) {
      sequenceFinished = true;  // Set sequenceFinished to true when countdown is over
      break;  // Exit the loop when sequenceFinished is true
    }


    for (int i = 0; i < sizeof(sequence) / sizeof(sequence[0]); i++){
      alarmScreen(sequenceFinished);  // update countdown

      digitalWrite(sequence[i], HIGH);
      delay(700);
      digitalWrite(sequence[i], LOW);
      delay(700);
    }
    delay(800);


    int i = 0;  // auxiliary variable
    while (i < 4) {

      alarmScreen(sequenceFinished);  // update countdown

      if(debounceButton(buttonBlue) == HIGH){
        digitalWrite(buttonBlue, LOW);
        pattern[i] = led_tab[0];
        digitalWrite(led_tab[0], HIGH);
        delay(500);
        digitalWrite(led_tab[0], LOW);
        delay(500);

        i++;  // update auxiliary variable
      }
      if(debounceButton(buttonGreen) == HIGH){
        digitalWrite(buttonGreen, LOW);
        pattern[i] = led_tab[1];
        digitalWrite(led_tab[1], HIGH);
        delay(500);
        digitalWrite(led_tab[1], LOW);
        delay(500);

        i++;  // update auxiliary variable
      }
      if (debounceButton(buttonRed) == HIGH){
        digitalWrite(buttonRed, LOW);
        pattern[i] = led_tab[2];
        digitalWrite(led_tab[2], HIGH);
        delay(500);
        digitalWrite(led_tab[2], LOW);
        delay(500);

        i++;  // update auxiliary variable
      }
    }

    // check if input is correct sequence
    bool match = true;
    for (int i = 0; i < sizeof(sequence)/sizeof(sequence[0]); i++){
      if(sequence[i] != pattern[i]){
        match = false;
        break;
      }
    }

    if(match){

      // 3 HIGH/LOW SHORT
      digitalWrite(led_tab[0], HIGH);
      digitalWrite(led_tab[1], HIGH);
      digitalWrite(led_tab[2], HIGH);
      delay(500);
      digitalWrite(led_tab[0], LOW);
      digitalWrite(led_tab[1], LOW);
      digitalWrite(led_tab[2], LOW);
      delay(500);
      digitalWrite(led_tab[0], HIGH);
      digitalWrite(led_tab[1], HIGH);
      digitalWrite(led_tab[2], HIGH);
      delay(500);
      digitalWrite(led_tab[0], LOW);
      digitalWrite(led_tab[1], LOW);
      digitalWrite(led_tab[2], LOW);
      delay(500);
      digitalWrite(led_tab[0], HIGH);
      digitalWrite(led_tab[1], HIGH);
      digitalWrite(led_tab[2], HIGH);
      delay(800);
      digitalWrite(led_tab[0], LOW);
      digitalWrite(led_tab[1], LOW);
      digitalWrite(led_tab[2], LOW);
      delay(800);

      sequenceFinished = true;
      // buzzer off

    } else {

      // 1 HIGH/LOW LONG
      digitalWrite(led_tab[0], HIGH);
      digitalWrite(led_tab[1], HIGH);
      digitalWrite(led_tab[2], HIGH);
      delay(1000);
      digitalWrite(led_tab[0], LOW);
      digitalWrite(led_tab[1], LOW);
      digitalWrite(led_tab[2], LOW);
      delay(500);

    }

  }

  // sequenceFinished, exit led display
  alarmScreen(sequenceFinished);

}



void alarmScreen(bool sequenceFinished){

  if (sequenceFinished) {
    // user woke up -> correct sequence + lightValue
    lcd.clear();
    lcd.print("Good morning!");
    sequenceFinished = true;
    alarmTime = false;
  } else {

    // handle countdown
    unsigned long currentMillis = millis();
    if (currentMillis - screenMillis >= 1000 && countdown > 0) {
      screenMillis = currentMillis;
      lcd.setCursor(0, 1);
      if (countdown < 10000) {
        lcd.print("0");
      }
      lcd.print(countdown / 1000);  // display in seconds
      lcd.print("s");
      lcd.write(2);

      countdown -= 1000;  // -1s
    } else if (countdown <= 0) {
      lcd.clear();
      lcd.print("Today is marked");
      lcd.setCursor(0, 1);
      lcd.print("with an X... ");
      lcd.write(3);
      sequenceFinished = true;
      alarmTime = false;
    }
  }
}