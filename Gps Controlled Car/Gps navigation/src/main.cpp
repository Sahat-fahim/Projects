#include <Arduino.h>
#include <AFMotor.h>
#include <Arduino_FreeRTOS.h>
#include <Wire.h>
#include <HMC5883L_Simple.h>
#include <TinyGPS++.h>
#include <math.h>

/* Motor  */
AF_DCMotor frontLEFT(3);
AF_DCMotor frontRIGHT(2);
AF_DCMotor backLEFT(4);
AF_DCMotor backRIGHT(1);
int Rspeed = 102+15;
int error = 48+15;
int Lspeed = Rspeed+error;

/* communication */
volatile bool Blue_condition= false;           // To send multiple data type to multiple variables 
volatile char RC_control;
volatile byte BT_controller;


/* Distance calculation  */
int trigPin= A3;
int echoPin= A2;
int distance;

/* Compass  */
float currentAngle, desiredAngle, angleError;
HMC5883L_Simple Compass;

/* GPS */
TinyGPSPlus GPS;
float currentLat, currentLon, givenLat, givenLon; 
float targetDistance;


/* Calculation  */
volatile byte cal;                             // for Switching betweeing calculation
const byte navigator = 0;
const byte directionFinder = 1;
const byte obj = 2; 


/* Free RTOS   */
void communucation(void *argc);
void readings(void *argc);                    // GPS, Compass, Distance
void calculation(void *argc);
TaskHandle_t Blue, sensor, calc, data;


/* Functions */
void angleAdjustment(); 
void carSTOP();
void carFORWARD();
void carBACKWARD();
void car_turn_Left();
void car_turn_Right();
void GPS_navigator();

void setup() {

  Serial.begin(9600);
  Serial2.begin(9600);                        // Bluetooth
  Serial1.begin(9600);                        // GPS
  Wire.begin();  
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);

  
  frontLEFT.setSpeed(Lspeed);
  frontRIGHT.setSpeed(Rspeed);
  backLEFT.setSpeed(Lspeed);   
  backRIGHT.setSpeed(Rspeed);

  Compass.SetDeclination(2,59,'E');
  Compass.SetSamplingMode(COMPASS_SINGLE);
  Compass.SetScale(COMPASS_SCALE_130);
  Compass.SetOrientation(COMPASS_HORIZONTAL_Y_NORTH);
  
  xTaskCreate(communucation, "Bluetooth", 512, NULL, 1, &Blue);   
  xTaskCreate(readings,"Sensor values", 128, NULL,1, &sensor);   
  xTaskCreate(calculation, "Calcutation & running", 512, NULL, 1, &calc); 
  xTaskCreate(printer, "sending information", 128, NULL, 1, &data);

  vTaskSuspend(calc); 

  vTaskStartScheduler();


  /*BT.println("       Welcome");
  BT.println("Type 0 for RC mode");
  BT.println("Type 1 for Longitude");
  BT.println("Type 2 for Latitude");
  BT.println("Type 3 for setting angle manually (not required when GPS coordinates available)");*/

}




void loop() {
   //
}

void printer(void *argc){
  
  (void) argc;

  while (1)
  {
      /*Serial2.print("Distance to nearest object:");
      Serial2.print(distance);
      Serial2.println("cm");
      vTaskDelay(1000/portTICK_PERIOD_MS); 

      Serial2.print("Angle: ");
      Serial2.println(currentAngle);
      vTaskDelay(1000/portTICK_PERIOD_MS);*/

      Serial2.print("Current Lat: ");  
      Serial2.print(currentLat,6);
      Serial2.print(" Current Lon: ");      
      Serial2.println(currentLon,6);
      vTaskDelay(2000/portTICK_PERIOD_MS);

      Serial2.print("Angle to location: ");
      Serial2.println(desiredAngle, 2); 
      vTaskDelay(2000/portTICK_PERIOD_MS);

      Serial2.print("Distance to location: ");
      Serial2.print(targetDistance, 2);
      Serial2.println(" Meters"); 
      vTaskDelay(2000/portTICK_PERIOD_MS);

  }
  


}

void communucation(void *argc){

    (void) argc;
    
    while (1)
    { 

     while(Serial2.available() > 0 && Blue_condition == false){
      BT_controller = Serial2.parseInt();
      Serial2.println(BT_controller);
      
      Blue_condition = true;
    }
   
    
     switch (BT_controller)
     {
     case 0:
        while (Serial2.available() > 0 && Blue_condition == true )
        {
         RC_control = Serial2.read();

         remoteControl(RC_control);
         if (RC_control == 'O')
         {
           Blue_condition = false;
         }       
        }
       break;

    case 1:

        while (Serial2.available() > 0 && Blue_condition == true )
        {
         givenLat = Serial2.parseFloat();
         Serial2.print("Lat:");
         Serial2.println(givenLat,6);
         Blue_condition = false; 
        }
        
       break;

     case 2:

        while (Serial2.available() > 0 && Blue_condition == true )
        {
         givenLon = Serial2.parseFloat();
         Serial2.print("Lon:");
         Serial2.println(givenLon, 6);
         vTaskResume(calc);
         Blue_condition = false; 
        }
        
       break;

      /*case 3:
        while (Serial2.available() > 0 && Blue_condition == true )
        {
         desiredAngle = Serial2.parseInt();
         Serial2.print("Desired angle:");
         Serial2.println(desiredAngle);
         Blue_condition = false; 
        }
       break;*/
      
      case 4:
        while (Serial2.available() > 0 && Blue_condition == true )
        {
           vTaskSuspendAll();
         Blue_condition = false; 
        }
        
        break;

     default:
     // nothing
       break;
     }
    }   
}



void readings(void *argc){

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
    currentAngle = Compass.GetHeadingDegrees();

    while (Serial1.available() > 0)
   {
     long c = Serial1.read();
      
      Serial.write(c);

      if (GPS.encode(c))
      {
        Serial.println();
        currentLat = GPS.location.lat();
        currentLon = GPS.location.lng(); 
        unsigned long fix_age = GPS.date.age(); 
        
      if (!GPS.location.isValid())
         
         Serial.println("Invalid fix");
      else if (fix_age > 2000)
        Serial.println("Stale fix");
      else
        Serial.println("Valid fix");
      }
   }

  }  
}


void calculation(void *argc){

  (void) argc;

  while (1)
  {
  
  switch (cal)
  {

  case navigator : 

     GPS_navigator();

    if ((targetDistance && desiredAngle) > 0){   // only allowed to switch to direction finder if gps cordinates are available 
      cal = directionFinder; 
    }
    break; 

  case directionFinder:
       angleAdjustment();
       vTaskDelay(2000/portTICK_PERIOD_MS);

       cal = obj;            // Send to object avoidance in order to responde obstacle  
    break;

  case obj:
     if (distance <= 40)
     {
      carSTOP();
     }else if (distance >= 40)
     {
      carFORWARD(); 
     }

  break; 
  
  default:
  //
    break;
  }
  } 
}


void angleAdjustment(){

  
  
  do
  {
    Serial2.print("Angle: ");
    Serial2.println(currentAngle);

    angleError = currentAngle - desiredAngle;

    if ((angleError < 0 && angleError > -180) || angleError > 180)
    {
      frontLEFT.setSpeed(120);  /* slowly turning right*/
      backRIGHT.setSpeed(100);    

      frontLEFT.run(BACKWARD);
      backRIGHT.run(FORWARD); 
    }else if (angleError < -180 || (angleError > 0 && angleError < 180 ))
    {
      frontRIGHT.setSpeed(120);  /* slowly turning left*/
      backLEFT.setSpeed(100); 

      frontRIGHT.run(BACKWARD);
      backLEFT.run(FORWARD); 
    }else if (abs(angleError) == 180 || angleError == 0)
    {
      frontLEFT.setSpeed(110);  /* if difference is same from both side turn right*/
      backRIGHT.setSpeed(90);    

      frontLEFT.run(BACKWARD);
      backRIGHT.run(FORWARD);
    }

  } while (abs(angleError) >= 10 );
  
   carSTOP();

}

void GPS_navigator(){

   /* const float radious= 6371.00; //Earth radious*/
   
    float p = M_PI/180.00;  

    
    desiredAngle= atan2(sin((givenLon-currentLon)*p)*cos(givenLat*p), cos(currentLat*p)*sin(givenLat*p) - sin(currentLat*p)*cos(givenLat*p)*cos((givenLon-currentLon)*p));
    
    desiredAngle = desiredAngle*(180.00/M_PI);  // angle in degrees
    if (desiredAngle < 0.00)
    {
      desiredAngle += 360.00; 
    }
    

   /* float a= 0.50 - cos((givenLat-currentLat)*p)/2.00 + cos(currentLat*p)*cos(givenLat*p) * (1.00-cos((givenLon-currentLon)*p)) / 2.00;
     targetDistance= (2.00*radious*asin(sqrt(a)))*1000.00;*/
     
     targetDistance = (double) TinyGPSPlus::distanceBetween(
          currentLat,
          currentLon,
          givenLat,
          givenLon
     );

}




void remoteControl(char RC_control){
   
    switch (RC_control)
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
     vTaskDelay(500/portTICK_PERIOD_MS);
     RC_control ='S';
   break;
   case 'L':
    car_turn_Left();
    vTaskDelay(500/portTICK_PERIOD_MS);
     RC_control ='S';
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

  frontLEFT.setSpeed(Lspeed/2);
  frontRIGHT.setSpeed(Rspeed/2);
  backLEFT.setSpeed(Lspeed/2);   
  backRIGHT.setSpeed(Rspeed/2);

  
  frontLEFT.run(FORWARD);
  frontRIGHT.run(BACKWARD);
  backLEFT.run(FORWARD);
  backRIGHT.run(BACKWARD);
}

void car_turn_Left(){

  frontLEFT.setSpeed(Lspeed/2);
  frontRIGHT.setSpeed(Rspeed/2);
  backLEFT.setSpeed(Lspeed/2);   
  backRIGHT.setSpeed(Rspeed/2);

  
  frontLEFT.run(BACKWARD);
  frontRIGHT.run(FORWARD);
  backLEFT.run(RELEASE);
  backRIGHT.run(FORWARD);
}