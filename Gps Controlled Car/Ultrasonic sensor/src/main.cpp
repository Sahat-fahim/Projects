#include <Arduino.h>

const byte trigPin= 9;
const byte echoPin= 11;

int travelTime;
float distance; 

void setup() {
  Serial.begin(9600);
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT); 

}

void loop() {
  digitalWrite(trigPin,LOW);
  delayMicroseconds(10);
  digitalWrite(trigPin,HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin,LOW); 
  travelTime = pulseIn(echoPin, HIGH);
  delay(20);

  distance = (0.0343*travelTime)/2; 
  Serial.print("Distance:");
  Serial.print(distance);
  Serial.println("cm");
  delay(1000);   
}