#include <Arduino.h>
#include <HMC5883L_Simple.h>
#include <AFMotor.h>
#include <SoftwareSerial.h>
#include <Wire.h>
#include <Arduino_FreeRTOS.h>

AF_DCMotor frontLEFT(3);
AF_DCMotor frontRIGHT(2);
AF_DCMotor backLEFT(4);
AF_DCMotor backRIGHT(1);
SoftwareSerial BT(14, 15);  //rx, tx 
HMC5883L_Simple Compass;


int Rspeed = 102+15;
int error = 48+15;

int Lspeed = Rspeed+error;
int currentAngle;
int desiredAngle = 20;

char controller; 

void compass(void *argc);
void communucation(void *argc);
void adjustAngle(void *argc);

TaskHandle_t com, Blue, Angle;


void carSTOP();
void carFORWARD();
void carBACKWARD();
void car_turn_Left();
void car_turn_Right(); 

void setup() {

  Serial.begin(9600);
  BT.begin(9600);
  Wire.begin();  
  


  frontLEFT.setSpeed(Lspeed);
  frontRIGHT.setSpeed(Rspeed);
  backLEFT.setSpeed(Lspeed);   
  backRIGHT.setSpeed(Rspeed);

  Compass.SetDeclination(2,59,'E');
  Compass.SetSamplingMode(COMPASS_SINGLE);
  Compass.SetScale(COMPASS_SCALE_130);
  Compass.SetOrientation(COMPASS_HORIZONTAL_Y_NORTH);
  
  xTaskCreate(adjustAngle, "Angle", 128, NULL, 1, &Angle);
  xTaskCreate(compass, "Compass", 128, NULL, 1, &com);
  xTaskCreate(communucation, "Bluetooth", 128, NULL, 1, &Blue);

  vTaskStartScheduler();

}

void loop() {
  
   //
}

void adjustAngle(void *argc){

  (void) argc;

  while (1)
  {
    if (controller == '1')
  {
  int currentBearing= currentAngle;

  int desiredBearing = currentAngle+20;
  if(desiredBearing > 359) {
    desiredBearing -= 359;
  }
  do
  {
    int newBearing = currentAngle;
    double desiredBearingSINE = sin(desiredBearing * (PI/180));
    double currentBearingSINE = sin(newBearing * (PI/180));
    
    if(desiredBearingSINE > 0 && currentBearingSINE < 0) {
     newBearing = newBearing - 360;
       }
     if(newBearing >= desiredBearing) {
      carSTOP();
      break;
    }
    else {
      car_turn_Left();
    }
  } while (1);


  }else if (controller == '0')
  {
  carSTOP();
  }
  
  }
}


void compass(void *argc){

  (void) argc;
   
   while (1)
   {
     currentAngle = Compass.GetHeadingDegrees();
     BT.println(currentAngle);
   }
}

void communucation(void *argc){

    (void) argc;
    
    while (1)
    {
      while (BT.available())
     {
     controller = BT.read();
     BT.println(controller);
     Serial.println(controller);  
     }
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

   frontLEFT.setSpeed(80);
   frontRIGHT.setSpeed(50);
   backLEFT.setSpeed(80);   
   backRIGHT.setSpeed(50);

  
  frontLEFT.run(FORWARD);
  frontRIGHT.run(BACKWARD);
  backLEFT.run(FORWARD);
  backRIGHT.run(BACKWARD);
}

void car_turn_Left(){

   frontLEFT.setSpeed(50);
   frontRIGHT.setSpeed(80);
   backLEFT.setSpeed(50);   
   backRIGHT.setSpeed(80);

  
  frontLEFT.run(BACKWARD);
  frontRIGHT.run(FORWARD);
  backLEFT.run(RELEASE);
  backRIGHT.run(FORWARD);
}