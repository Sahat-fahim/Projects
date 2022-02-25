#include <Arduino.h>
#include <AFMotor.h>

AF_DCMotor frontLEFT(3);
AF_DCMotor frontRIGHT(2);
AF_DCMotor backLEFT(4);
AF_DCMotor backRIGHT(1);

int Rspeed = 136;
int Lspeed = 200;

char controller; 


void carSTOP();
void carFORWARD();
void carBACKWARD();
void car_turn_Left();
void car_turn_Right(); 

void setup() {

  Serial.begin(9600);

  frontLEFT.setSpeed(Lspeed);
  frontRIGHT.setSpeed(Rspeed);
  backLEFT.setSpeed(Lspeed);   
  backRIGHT.setSpeed(Rspeed);
}

void loop() {
   
   while (Serial.available() > 0)
   {
     controller = Serial.read();
     Serial.println(controller);  
   }
   
   switch (controller)
   {
   case 'F':
    carFORWARD();
     break;
   case 'B': 
    carBACKWARD();
     break;
   case 'S':
     carSTOP();
     break;
   case 'R': 
     car_turn_Right();
   break;
   case 'L':
    car_turn_Left();
    break;

   default: 
     carSTOP();   
     break;
   }       
}

void carSTOP(){
    
  frontLEFT.run(RELEASE);
  frontRIGHT.run(RELEASE);
  backLEFT.run(RELEASE);
  backRIGHT.run(RELEASE);
}

void carFORWARD(){
  frontLEFT.run(FORWARD);
  frontRIGHT.run(FORWARD);
  backLEFT.run(FORWARD);
  backRIGHT.run(FORWARD);
}

void carBACKWARD(){   
  frontLEFT.run(BACKWARD);
  frontRIGHT.run(BACKWARD);
  backLEFT.run(BACKWARD);
  backRIGHT.run(BACKWARD);
}

void car_turn_Right(){
  
  frontLEFT.run(FORWARD);
  frontRIGHT.run(RELEASE);
  backLEFT.run(FORWARD);
  backRIGHT.run(RELEASE);
}

void car_turn_Left(){
  
  frontLEFT.run(RELEASE);
  frontRIGHT.run(FORWARD);
  backLEFT.run(RELEASE);
  backRIGHT.run(FORWARD);
}