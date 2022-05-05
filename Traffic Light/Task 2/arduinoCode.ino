#define GREEN 5
#define YELLOW 6
#define RED 7
#define pRED 9
#define pGREEN 8

int state=0 ;
int pushButton= 0;
int buttonPin= 2;
int pState=0;



void setup()
{
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
  if(pushButton == 1){
  redON();
  delay(2000);
  pedestrianLight();
  delay(2000);
    
  pushButton =0;
  }else{
  trafficLight();
  }
}






void pedestrianLight()
{ 
  pState= pRED;
  
  switch(pState)
  {
   case pRED:
   digitalWrite(pRED, LOW);
   
   case pGREEN:
   pGREENon();
   
  }
 
  
}



void trafficLight()
{
  
  state= RED;
  
  switch (state)
  {
    
	case RED:
	redON();
    delay(2000);
    
    case YELLOW:
    yellowON();
    delay(2000);
       
    case GREEN:
	greenON();
    delay(2000);
  
    yellowON();
    delay(2000);
    
    
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


void pGREENon()
{
  digitalWrite(pGREEN, HIGH);
  delay(3000);
  digitalWrite(pGREEN, LOW);
  digitalWrite(pRED, HIGH);
  
}


void Interruption()
{
  (pushButton=1);
}
