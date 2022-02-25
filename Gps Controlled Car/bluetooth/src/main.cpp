#include <Arduino.h>
#include <SoftwareSerial.h>

char incomingvalue;
const byte led= 13;

void setup() {
 Serial.begin(9600);
 pinMode(led,OUTPUT);

}

void loop() {
   
   while(Serial.available() > 0){
     incomingvalue = Serial.read();
     Serial.println(incomingvalue);
     }
   
   if (incomingvalue == '1')
   {
     digitalWrite(led,HIGH);
   }else if (incomingvalue == '0')
   {
     digitalWrite(led,LOW);
   }
   
}