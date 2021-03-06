#include <Arduino.h>
#include <AFMotor.h>
#include <Arduino_FreeRTOS.h>
#include <SoftwareSerial.h>

const byte go = 0;
const byte brake = 1;
const byte right = 2;

volatile int state;

AF_DCMotor frontLEFT(3);
AF_DCMotor frontRIGHT(2);
AF_DCMotor backLEFT(4);
AF_DCMotor backRIGHT(1);
SoftwareSerial BT(14, 15);  //rx, tx 

int trigPin= A3;
int echoPin= A2;

int Rspeed = 174;
int Lspeed = 255;

char controller; 
int distance; 


void carSTOP();
void carFORWARD();
void carBACKWARD();
void car_turn_Left();
void car_turn_Right(); 

void carRUN(void *argc);
void Blue(void *argc);
void distanceCAL(void *argc);

TaskHandle_t bluetooth;
TaskHandle_t car;
TaskHandle_t ultra;

void setup(){
   
  Serial.begin(9600);
  BT.begin(9600);
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);

  frontLEFT.setSpeed(Lspeed);
  frontRIGHT.setSpeed(Rspeed);
  backLEFT.setSpeed(Lspeed);   
  backRIGHT.setSpeed(Rspeed);

  xTaskCreate(Blue,"communication", 128, NULL,1, &bluetooth);
  xTaskCreate(carRUN,"Running", 128, NULL,1, &car);
  xTaskCreate(distanceCAL,"Mesuring distance", 128, NULL,1, &ultra);

  vTaskStartScheduler();
}

void loop (){
   ///
}


void Blue(void *argc){


  (void) argc;
    
    while (1)
    {
      while (BT.available())
   {
     controller = BT.read();
     BT.println(controller);
   }
 }
}

void distanceCAL(void *argc){

  (void) argc;

  while (1)
  {
    digitalWrite(trigPin,LOW);
    delayMicroseconds(10);
    digitalWrite(trigPin,HIGH);
    delayMicroseconds(10);
    digitalWrite(trigPin,LOW); 
    int travelTime = pulseIn(echoPin, HIGH);
    vTaskDelay(10/portTICK_PERIOD_MS);

    distance = (0.0343*travelTime)/2; 
    BT.print("Distance:");
    BT.print(distance);
    BT.println("cm");
    vTaskDelay(10/ portTICK_PERIOD_MS); 
  }  
}


void carRUN(void *argc){

  (void) argc;

    while (1)
  {
    if (controller == '1') {
     

   switch (state)
   {
   case go:
       carFORWARD();
       //
       if (distance <= 40)
       {
        state = brake; 
       }   
     break;
     
   case brake:
      carSTOP();
      vTaskDelay(500/ portTICK_PERIOD_MS);
      
      state = right;
     break;
     
   case right:
       
      car_turn_Right();
      vTaskDelay(500/ portTICK_PERIOD_MS);

     if (distance >= 40){
       carSTOP();
       vTaskDelay(500/ portTICK_PERIOD_MS);
       state = go;
      }
    break;

   default:
   //
     break;
   }

      

     
    }else if (controller == '0')
    {
      carSTOP();
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