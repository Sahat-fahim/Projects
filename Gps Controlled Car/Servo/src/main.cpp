#include <Arduino.h>
#include <Servo.h>
#include <AFMotor.h>

Servo myservo;


void setup() {
  myservo.attach(10);
}

void loop() {
  
  for(int i = 0; i<180; i++){
   
   myservo.write(i);
   delay(100);
  }

  for (int i = 180-1; i >0; i--)
  {
   myservo.write(i);
   delay(100);
  }
  
  
  
}