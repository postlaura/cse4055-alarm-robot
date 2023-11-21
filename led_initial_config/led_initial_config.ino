const int BUTTON1 = 5; 
const int BUTTON2 = 6; 
const int BUTTON3 = 7; 
const int LED1 = 9; // blue
const int LED2 = 10; // green 
const int LED3 = 11; // red 

int BUTTONstate1 = 0;  
int BUTTONstate2 = 0;  
int BUTTONstate3 = 0;  

void setup() {
  pinMode(BUTTON1, INPUT); 
  pinMode(BUTTON2, INPUT); 
  pinMode(BUTTON3, INPUT); 
  pinMode(LED1, OUTPUT);   
  pinMode(LED2, OUTPUT);   
  pinMode(LED3, OUTPUT);   
  Serial.begin(9600);
}

void loop() {
  BUTTONstate1 = digitalRead(BUTTON1);
  if (BUTTONstate1 == HIGH) {
    digitalWrite(LED1, HIGH);
  } else {
    digitalWrite(LED1, LOW);
  }
  Serial.println(BUTTONstate1);

  BUTTONstate2 = digitalRead(BUTTON2);
  if (BUTTONstate2 == HIGH) {
    digitalWrite(LED2, HIGH);
  } else {
    digitalWrite(LED2, LOW);
  }
  Serial.println(BUTTONstate2);
  
  BUTTONstate3 = digitalRead(BUTTON3);
  if (BUTTONstate3 == HIGH) {
    digitalWrite(LED3, HIGH);
  } else {
    digitalWrite(LED3, LOW);
  }
  Serial.println(BUTTONstate3);

  delay(100);  // Add a 100-millisecond delay
  Serial.println("finish");
}
