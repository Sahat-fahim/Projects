//Master

#include <Wire.h>

#define GREEN 13
#define YELLOW 12
#define RED 11
#define pRED 8
#define pGREEN 9

volatile int state = RED;
volatile int pState = pRED;
volatile int x=0;
volatile int y;

int Delay= 1000;

int pushButton= 0;
int buttonPin= 2;

volatile int TL_condition;

void setup()
{
  Wire.begin();
  Serial.begin(9600);
  
  pinMode(GREEN, OUTPUT);
  pinMode(YELLOW, OUTPUT);
  pinMode(RED, OUTPUT);
  
  pinMode(pRED, OUTPUT);
  pinMode(pGREEN, OUTPUT);
  pinMode(buttonPin,INPUT);
  
  digitalWrite(pRED, HIGH);
  
  attachInterrupt(digitalPinToInterrupt(buttonPin),Interruption,CHANGE);
}  


void loop()
{    
  switch(x)
  { 
   case 0:   
    
     masterLane();
     
    
    if(TL_condition == 1){
      Wire.beginTransmission(1); 
      Wire.write(1);
      Wire.endTransmission();
      x=1;}
          
   break;
        
   case 1:
    
     redON();
     
     Wire.requestFrom(1,1);
     while (Wire.available()){
     y= Wire.read();
     Serial.println(y);}
     
    if(y == 2)
    {
     Wire.beginTransmission(1); 
     Wire.write(0);
     Wire.endTransmission();
      
     delay(Delay);
      x=0;}
  
   break;
   }       
}

void masterLane()
{ 
  if(pushButton == 1){
  redON();
  delay(Delay);
    
  pedestrianLight();
  delay(Delay);
    
  pushButton =0;
  }else {    
  trafficLight();
  }
}





void trafficLight()
{ 
  switch (state)
  {
    
	case RED:
	redON();
    delay(Delay);
    
    case YELLOW:
    yellowON();
    delay(Delay);
       
    case GREEN:
	greenON();
    delay(Delay);
  
    yellowON();
    delay(Delay);   
  } 
  TL_condition = 1;
}

void pedestrianLight()
{ 
  
  switch(pState)
  {
   case pRED:
   digitalWrite(pRED, LOW);
   
   case pGREEN:
   pGREENon();
   
  }
  TL_condition = 0;
}


void greenON()
{
  digitalWrite(GREEN, HIGH);
  digitalWrite(YELLOW, LOW);
  digitalWrite(RED, LOW);
}

void yellowON()
{
  digitalWrite(GREEN, LOW);
  digitalWrite(YELLOW, HIGH);
  digitalWrite(RED, LOW);
}

void redON()
{
  digitalWrite(GREEN, LOW);
  digitalWrite(YELLOW, LOW);
  digitalWrite(RED, HIGH);
}

void pGREENon()
{
  digitalWrite(pGREEN, HIGH);
  delay(Delay);
  digitalWrite(pGREEN, LOW);
  digitalWrite(pRED, HIGH);
  
}

void Interruption()
{
  (pushButton=1);
}




// slave 
#include <Wire.h>
#define RED 13
#define YELLOW 12
#define GREEN 11


volatile int state = RED;

int y;
volatile int x;

void setup()
{
  Wire.begin(1);                
  Wire.onReceive(receiveEvent);
  Wire.onRequest(requestEvent);
  Serial.begin(9600);
  
  pinMode(RED, OUTPUT);
  pinMode(YELLOW, OUTPUT);
  pinMode(GREEN, OUTPUT);
}


void loop()
{ 
  switch (x){
    
    case 0: 
         redON();
        
    y = 0; 
        break;
    case 1:
    trafficLight();
    
    y = 1;
    delay(500);
    
    x = 0;
    break;
  }

}


void receiveEvent(int howMany)
{
  while(Wire.available()) 
  {
    x = Wire.read(); 
    Serial.println(x);
  }
}

void requestEvent()
{
   switch(y)
    { 
    case 0:
    Wire.write(3);
     break;
     
    case 1:
    Wire.write(2);
    break;
    }
}

void trafficLight()
{ 
  
  switch (state)
  {    
	case RED:
	redON();
    delay(1000);
    
    case YELLOW:
    yellowON();
    delay(1000);
       
    case GREEN:
	greenON();
    delay(1000);
    yellowON();
    delay(1000);
   }
}


void greenON()
{
  digitalWrite(GREEN, HIGH);
  digitalWrite(YELLOW, LOW);
  digitalWrite(RED, LOW);
}

void yellowON()
{
  digitalWrite(GREEN, LOW);
  digitalWrite(YELLOW, HIGH);
  digitalWrite(RED, LOW);
}

void redON()
{
  digitalWrite(GREEN, LOW);
  digitalWrite(YELLOW, LOW);
  digitalWrite(RED, HIGH);
}