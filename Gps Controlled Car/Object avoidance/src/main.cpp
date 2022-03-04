#include <Arduino.h>
#include <AFMotor.h>
#include <Arduino_FreeRTOS.h>
#include <SoftwareSerial.h>

const byte go = 0;
const byte comparator = 1;
const byte right = 2;
const byte left = 3;
const byte reverse = 4;

volatile int Direction;

AF_DCMotor frontLEFT(3);
AF_DCMotor frontRIGHT(2);
AF_DCMotor backLEFT(4);
AF_DCMotor backRIGHT(1);
SoftwareSerial BT(14, 15);  //rx, tx 

int trigPin= A3;
int echoPin= A2;

int Rspeed = 174;
int error = 81;

int Lspeed = Rspeed+error;

char controller; 
int distance, rightDistance, leftDistance; 


void carSTOP();
void carFORWARD();
void carBACKWARD();
void car_turn_Left();
void car_turn_Right(); 
void carNarrow();

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
  xTaskCreate(carRUN,"Running", 256, NULL, 1, &car);
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
     
  if (distance >= 80)
  {
    carFORWARD(); 

  }else if (distance <= 80)
  {
   carNarrow();
  }
  
  
}else if (controller == '0')
    {
      carSTOP();
    }
  }     
}




void carNarrow(){

  // function to operate the car in a narrow space

 switch (Direction)
   {
   case go:
       carFORWARD();
       vTaskDelay(500/ portTICK_PERIOD_MS);
       carSTOP();
       vTaskDelay(500/ portTICK_PERIOD_MS);
       if (distance <= 40)
       {
        Direction = comparator; 
       }   
     break;
     
   case comparator:
      
       car_turn_Right();
       rightDistance= distance;   // turning the car towards right and checking right side distance
       vTaskDelay(500/ portTICK_PERIOD_MS);

       car_turn_Left();
       vTaskDelay(500/ portTICK_PERIOD_MS);
       car_turn_Left();
       leftDistance = distance;
       vTaskDelay(500/ portTICK_PERIOD_MS);  //turnig left twice to fully turn left
      
       if (rightDistance > leftDistance)
       {
         Direction = right;
       }else if (leftDistance > rightDistance)
       {
         Direction = left;
       }else if (leftDistance == rightDistance)
       {
         Direction = reverse;
       }
     break;
     
   case right:
  
      car_turn_Right();
      vTaskDelay(500/ portTICK_PERIOD_MS);

     if (distance <= 40){
       carSTOP();
       vTaskDelay(500/ portTICK_PERIOD_MS);
       Direction = reverse;
      }else if (distance >= 40)
      {
        carSTOP();
        vTaskDelay(500/ portTICK_PERIOD_MS);
        Direction = go;
      }    
    break;

   case left:
     
        car_turn_Left();  
        vTaskDelay(500/ portTICK_PERIOD_MS);
   
     if (distance <= 40){
       carSTOP();
       vTaskDelay(500/ portTICK_PERIOD_MS);
       Direction = reverse;
     }else if (distance >= 40)
     {
       carSTOP();
       vTaskDelay(500/ portTICK_PERIOD_MS);
       Direction = go;
     }
    
   break;

   case reverse:
    
    for (int i = 0; i < 2; i++)
    {
      carBACKWARD();
    }

    vTaskDelay(500/ portTICK_PERIOD_MS);
    Direction = comparator;

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

  frontLEFT.setSpeed(255);
  frontRIGHT.setSpeed(255);
  backLEFT.setSpeed(255);   
  backRIGHT.setSpeed(255);
  
  frontLEFT.run(FORWARD);
  frontRIGHT.run(BACKWARD);
  backLEFT.run(FORWARD);
  backRIGHT.run(BACKWARD);
}

void car_turn_Left(){

  frontLEFT.setSpeed(255);
  frontRIGHT.setSpeed(255);
  backLEFT.setSpeed(255);   
  backRIGHT.setSpeed(255);
  
  frontLEFT.run(BACKWARD);
  frontRIGHT.run(FORWARD);
  backLEFT.run(BACKWARD);
  backRIGHT.run(FORWARD);
} 