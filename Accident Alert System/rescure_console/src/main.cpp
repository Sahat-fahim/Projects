#include <SPI.h>
#include <WiFiNINA.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>
#include <Servo.h>
#include "super_secret.h" 


const char* ssid = super_secretSSID;
const char* password = super_secretPASS;
const int buzzerPin = 8;
const int ledPin = 7;

const char* mqttServer = "192.168.103.242";
const int mqttPort = 1883;

WiFiClient wifiClient;
PubSubClient client(wifiClient);
Servo myservo;

bool recv_status = false;
const char* topic = "Hospital/rescue_console";
char rx_buf[1024]; 

//const char* topic2 = "Car/ /GPS_Data";
StaticJsonDocument<200> doc;

void json_parser(char *buf);

void reconnect() {
  while (!client.connected()) {
    if (client.connect("rescue_client")) {
      Serial.println("Reconnected to MQTT broker!");
      client.subscribe(topic);
      //client.subscribe(topic2);
    } else {
      Serial.println("Failed to reconnect to MQTT broker. Retrying in 5 seconds...");
      delay(5000);
    }
  }
}

void messageCallback(char* topic, byte* payload, unsigned int length) {

  Serial.println("Accident Detected!!");
  for (int i = 0; i < length; i++) {
    rx_buf[i] = payload[i];
  }
  // Serial.print("Rx Buf: ");
  // Serial.println(rx_buf);
  recv_status = true;

}

void rescueAction(double lat, double lon){

  // This funcion will do the dispatch drone and alert ambulance 
  
  
  digitalWrite(ledPin, HIGH);

  // Run the buzzer
  tone(buzzerPin, 1000, 5000);

  // Run servo
  for(int i = 0; i<180; i++){  
   myservo.write(i);
   delay(100);
  }

  for (int i = 180-1; i >0; i--){
   myservo.write(i);
   delay(100);
  }
  digitalWrite(ledPin, LOW);
  delay(1000);

}


void json_parser(char *json){

   DeserializationError error = deserializeJson(doc, json);

  // Test if parsing succeeds.
  if (error) {
    Serial.print(F("deserializeJson() failed: "));
    Serial.println(error.f_str());
    return;
  }


  double latitude = doc["latitude"];
  double longitude = doc["longitude"];

  Serial.print("Latitude: ");
  Serial.print(latitude, 6);
  Serial.print(" Longitude: ");
  Serial.println(longitude, 6);

  rescueAction(latitude, longitude); 
}


void setup() {

  Serial.begin(9600);

  WiFi.begin(ssid, password); // Connect to Wi-Fi
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }
  Serial.println("Connected to WiFi!");

  client.setServer(mqttServer, mqttPort);   // setting mqtt broker
  client.setCallback(messageCallback);
  myservo.attach(10);
  pinMode(buzzerPin, OUTPUT);
  pinMode(ledPin, OUTPUT);

  // Connect to MQTT broker
  while (!client.connected()) {
    if (client.connect("rescue_client")) {
      Serial.println("Connected to MQTT broker!");
      client.subscribe(topic);
    } else {
      Serial.println("Failed to connect to MQTT broker. Retrying in 5 seconds...");
      delay(5000);
    }
  }
}

void loop() {

  int ret;
  if (!client.connected()) {    // Maintain the MQTT connection
    reconnect();
  }
  client.loop();
  
  if (recv_status == true)
  {
    json_parser(rx_buf);
    recv_status = false;
  }  
  
  delay(1000);
  
}


