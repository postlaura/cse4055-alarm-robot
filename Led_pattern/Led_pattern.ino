#include <RTClib.h> // Inclure la bibliothèque RTClib
RTC_DS3231 rtc;

DateTime alarm(2023, 11, 9, 7, 57, 0); // Déclaration de la variable alarm en dehors de setup()

const int buzzer = 11; //buzzer to arduino pin 9

//pin of the buttons

int const b1 = 5;
int const b2 = 6;
int const b3 = 7;

// pin of the LEDs

int const l1 = 8;
int const l2 = 9;
int const l3 = 10;

int const led_tab[3] = {l1, l2, l3};

int sequence[4]; // Déclaration d'un tableau pour stocker la séquence de 4 nombres aléatoires
int pattern[4];

bool alarm_on = false;

void setup() {
  Serial.begin(9600); // Initialisation de la communication série pour afficher les nombres générés
  randomSeed(analogRead(0)); // Initialisation du générateur de nombres aléatoires avec une valeur analogique aléatoire
  rtc.begin();
}



void loop() {


  // rtc.now == alarm
  if (true){
    alarm_on = true;
    for (int i = 0; i < 4; i++) {
      sequence[i] = led_tab[random(0, 3)]; 
    }
    tone(buzzer, 1000);
  }

  
  while(alarm_on) {


    for (int i = 0; i < sizeof(sequence) / sizeof(sequence[0]); i++){
      digitalWrite(sequence[i], HIGH);
      delay(500);
      digitalWrite(sequence[i], LOW);
      Serial.println(sequence[i]);
      }


    bool flag = false;
    int i = 0;
    
    while(i < 4){
      
      if(digitalRead(b1) == HIGH){
        Serial.println("button1 is on");
        pattern[i] = led_tab[0];
        i++;
        digitalWrite(led_tab[0], HIGH);
        delay(100);
        digitalWrite(led_tab[0], LOW);
        
      }
      if(digitalRead(b2) == HIGH){
        Serial.println("button2 is on");
        pattern[i] = led_tab[1];
        i++;
        
        digitalWrite(led_tab[1], HIGH);
        delay(100);
        digitalWrite(led_tab[1], LOW);
        
      }
      if (digitalRead(b3) == HIGH){
        Serial.println("button3 is on");
        pattern[i] = led_tab[2];
        i++;
        digitalWrite(led_tab[2], HIGH);
        delay(100);
        digitalWrite(led_tab[2], LOW);
      }
      
      
    }
    
 
   }

    bool match = true;
    for (int j = 0; j < sizeof(sequence)/sizeof(sequence[0]); j++){
      if(sequence[j] != pattern[j]){
        match = false;
        break;
      }
    }

    if(match){
      alarm_on = false;
      noTone(buzzer); 
      }
    

  delay(1000); // Attente d'une seconde entre chaque génération de séquence
}
