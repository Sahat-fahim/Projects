#define GREEN 5
#define YELLOW 6
#define RED 7
#define pRED 9
#define pGREEN 8

volatile int state= pRED;
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
  
  
  attachInterrupt(digitalPinToInterrupt(buttonPin),Interruption,CHANGE);
  
}


void loop()
{  
  switch (state)
  {
    
    case pRED:
    
    if(pushButton == 1){
    pREDoff();
    state= pGREEN;
    }else {
    digitalWrite(pRED, HIGH);
    state= RED;}
    break;
    
    case RED:
    redON();
    delay(2000);
    
    if (pushButton == 1)
    state= pRED;  
    else{
    state=YELLOW;}
    break;
    
    case YELLOW:
    yellowON();
    delay(2000);
    state= GREEN;
    break;
       
    case GREEN:
    greenON();
    delay(2000);
  
    yellowON();
    delay(2000);
    state=RED;
    break;
    
    case pGREEN:
    pGREENon();
    pushButton=0;
    state= pRED;
    break;
   
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
  
}

void pREDoff()
{
  digitalWrite(pRED, LOW);
}

void Interruption()
{
  (pushButton=1);
}
