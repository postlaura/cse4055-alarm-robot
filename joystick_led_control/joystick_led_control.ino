int mid =10;
int z1 =0;
int z2 = 0;
int z3 = 0;

void setup() {
  pinMode(12,OUTPUT); // red LED
  pinMode(10,OUTPUT); // green LED
  pinMode(8,OUTPUT);  // blue LED

  // joystick inputs
  pinMode(A0,INPUT);
  pinMode(A1,INPUT);
  pinMode(A2, INPUT);

  // light LED
  digitalWrite(12, HIGH);
  digitalWrite(10, HIGH);
  digitalWrite(8, HIGH);

  z1 = analogRead(A2); 
  z2 = analogRead(A1); 
  z3 = analogRead(A0);
}

void loop() {
  int xAxis = analogRead(A2); // Read Joysticks X-axis
  int yAxis = analogRead(A1); // Read Joysticks Y-axis
  int buttonJoystick = analogRead(A0); // Read button Joystick

  // Y-axis used for forward and backward control
  if (yAxis < z2 - mid) 
  {
    digitalWrite(12, HIGH);
  }
  
  else   
  {
     digitalWrite(12, LOW);
  }

  if (yAxis > z2 + mid) 
  {
    digitalWrite(10, HIGH);
  }
  
  else   
  {
     digitalWrite(10, LOW);
  }
  
  if (xAxis < z1 - mid) 
  {
    digitalWrite(8, HIGH);
  }
  
  else   
  {
     digitalWrite(8, LOW);
  }

  if (buttonJoystick == 1){
    digitalWrite(12, HIGH);
    digitalWrite(10, HIGH);
    digitalWrite(8, HIGH);
  }
  
  // we don't have 4th LED
  //if (xAxis > z1 + mid) 
  //{
  //  digitalWrite(7, HIGH);
  //}
  //
  //else   
  //{
  //   digitalWrite(7, LOW);
  //}
   
}
