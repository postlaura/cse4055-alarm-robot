#include <LiquidCrystal_I2C.h>
#include <ThreeWire.h>
#include <RtcDS1302.h>

LiquidCrystal_I2C lcd(0x27,16,2);  // set the LCD address to 0x27 for a 16 chars and 2 line display
ThreeWire myWire(5,3,7); //(DAT, CLK, RST)
RtcDS1302<ThreeWire>Rtc(myWire);

void setup() {
  // LCD setup
  lcd.init();
  lcd.backlight();
  lcd.clear();

  // RTC setup
  Rtc.Begin();

  // get current time
  // only run once
  //RtcDateTime currentTime= RtcDateTime(__DATE__, __TIME__);
  //Rtc.SetDateTime(currentTime);
}

void loop() {
  RtcDateTime now = Rtc.GetDateTime();

  lcd.clear();
  lcd.setCursor(0, 0);

  // print date in DD-MM-YYYY format
  lcd.print(now.Day());
  lcd.print("-");
  lcd.print(now.Month());
  lcd.print("-");
  lcd.print(now.Year());

  //lcd.setCursor(, )




  //lcd.setCursor(0,0);
  //lcd.print("Date: ");
  //lcd.print(now.Day());
  //lcd.print("/");
  //lcd.print(now.Month());
  //lcd.print("/");
  //lcd.print(now.Year());
//
  //lcd.setCursor(0, 1);
  //lcd.print("Time: ");
  //lcd.print(now.Hour());
  //lcd.print(":");
  //lcd.print(now.Minute());
  ////lcd.print(":");
  ////lcd.print(now.Second());
//
  delay(500);
}
