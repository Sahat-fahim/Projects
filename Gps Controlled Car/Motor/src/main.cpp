#include <Arduino.h>
#include <AFMotor.h>

AF_DCMotor frontLEFT(3);
AF_DCMotor frontRIGHT(2);
AF_DCMotor backLEFT(4);
AF_DCMotor backRIGHT(1);

int Rspeed = 136;
int Lspeed = 200;

void motorSTOP();

void setup() {
  // put your setup code here, to run once:
}

void loop() {
   
   frontLEFT.setSpeed(Lspeed);
   frontLEFT.run(FORWARD);
   frontRIGHT.setSpeed(Rspeed);
   frontRIGHT.run(FORWARD);
   backLEFT.setSpeed(Lspeed);
   backLEFT.run(FORWARD);
   backRIGHT.setSpeed(Rspeed);
   backRIGHT.run(FORWARD);

   delay(2000); 
   motorSTOP();
   delay(2000);
       
}

void motorSTOP(){
    
  frontLEFT.run(RELEASE);
  frontRIGHT.run(RELEASE);
  backLEFT.run(RELEASE);
  backRIGHT.run(RELEASE);
}