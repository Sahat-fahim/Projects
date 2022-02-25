//Master 

#include <Wire.h>

#define GREEN 13
#define YELLOW 12
#define RED 11
#define pRED 8
#define pGREEN 9

volatile int state = RED;
volatile int pState = pRED;
volatile int traffic_CONTROLLER=1;


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
  switch(traffic_CONTROLLER)
  { 
   case 1:   
    
     masterLane();
     
    
    if(TL_condition == 1)
    
    { Wire.beginTransmission(1); 
      Wire.write(2);
      Wire.endTransmission();
     
     traffic_CONTROLLER=2;}
          
   break;
        
   case 2:
    
     redON();
     
     Wire.requestFrom(1,1);
     while (Wire.available()){
     traffic_CONTROLLER= Wire.read();
     Serial.println(traffic_CONTROLLER);}
     
   
  
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





//Slave 

#include <Wire.h>
#define t10 1000
#define t20 1000
#define redState 1
#define yellowState 2
#define greenState 3

int trRED= 13;
int trYELLOW= 12;
int trGREEN= 11;
int pdRED= 8; 
int pdGREEN= 9; 
int pdBUTTON = 2; 

volatile byte buttonState= 0; 
int state= redState; 
int lastState;
volatile int traffic_CONTROLLER =1; 
volatile int TL_condition;



void setup()
{
  Wire.begin(1);                
  Wire.onReceive(receiveEvent);
  Wire.onRequest(requestEvent);
  Serial.begin(9600);
  
  pinMode(trRED, OUTPUT);
  pinMode(trYELLOW, OUTPUT);
  pinMode(trGREEN, OUTPUT);
  pinMode(pdGREEN, OUTPUT);
  pinMode(pdRED, OUTPUT);
  pinMode(pdBUTTON, INPUT);
  
  attachInterrupt(digitalPinToInterrupt(pdBUTTON), pdREQUEST, RISING);
  
  digitalWrite(pdRED, HIGH); 
  digitalWrite(pdGREEN, LOW);
}


void loop(){
  switch(traffic_CONTROLLER){
    
  case 1: laneSTOP();
  break; 
    
  case 2: laneGO();
    
    if(TL_condition == 1){ 
    traffic_CONTROLLER = 1;}
    break; 
    
  }
}

void carLANE(){
  
digitalWrite(pdGREEN, LOW);
digitalWrite(pdRED, HIGH);
  

  ledCONTROL(trRED, trYELLOW, trGREEN, HIGH, LOW, LOW, t20);
  ledCONTROL(trRED, trYELLOW, trGREEN, LOW, HIGH, LOW, t10);
  ledCONTROL(trRED, trYELLOW, trGREEN, LOW, LOW, HIGH, t20);
  ledCONTROL(trRED, trYELLOW, trGREEN, LOW, HIGH, LOW, t10);

}

void pedastrianCROSS(){
  
digitalWrite(pdGREEN, HIGH);
digitalWrite(pdRED, LOW);
ledCONTROL(trRED, trYELLOW, trGREEN, HIGH, LOW, LOW, t20);
buttonState=0; 
}

void pdREQUEST(){
  buttonState=1; 
}

void ledCONTROL(int tL1,int tL2,int tL3, int s1,int s2,int s3, int time)
{ 
  digitalWrite(tL1,s1);
  digitalWrite(tL2,s2);
  digitalWrite(tL3,s3);
  delay(time);
}
void laneSTOP()
{
  digitalWrite(trGREEN, LOW);
  digitalWrite(trYELLOW, LOW);
  digitalWrite(trRED, HIGH);}

void laneGO()
{
  switch(buttonState){
  case 0: carLANE();
    TL_condition=1; 
    break; 
  case 1: pedastrianCROSS();
    TL_condition=0;
    break; 
  }}
  

void receiveEvent(int howMany)
  {
  while(Wire.available()) 
  {
    traffic_CONTROLLER = Wire.read(); 
    Serial.println(traffic_CONTROLLER);
  }}
  
void requestEvent()
{
    Wire.write(traffic_CONTROLLER);
     
}
