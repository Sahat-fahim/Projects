#include <SPI.h>
#include <WiFiNINA.h>
#include <PubSubClient.h>
#include <Arduino_LSM6DS3.h>
#include <TinyGPS++.h>
#include "super_secret.h" 

#define CAR_ID "Car_A"
#define THRESHOLD 2.0
float initialAcc = 0;

const char* ssid = super_secretSSID;
const char* password = super_secretPASS;


const char* mqttServer = "192.168.103.242";
const int mqttPort = 1883;

WiFiClient wifiClient;
PubSubClient client(wifiClient);

TinyGPSPlus GPS;

const char* info_topic = "info_Car";
const char* topic = CAR_ID"/EDR/IMU_Data";
const char* topic2 = CAR_ID"/EDR/GPS_Data";

void reconnect() {
  while (!client.connected()) {
    if (client.connect("EDR_client")) {
      Serial.println("Reconnected to MQTT broker!");
      client.subscribe(topic);
      client.subscribe(topic2);
      client.subscribe (info_topic);
    } else {
      Serial.println("Failed to reconnect to MQTT broker. Retrying in 5 seconds...");
      delay(5000);
    }
  }
}

float get_acceleration(void){

  float x, y, z, acceleration;

  if (IMU.accelerationAvailable()) {
    IMU.readAcceleration(x, y, z);
    acceleration = sqrt(x*x + y*y + z*z);
    return acceleration;
  }
  return -101.0;
}

int get_GPS(void){

  double latitude, longitude;
  char latStr[128], lonStr[128], buf[256];

  // Hard coded GPS coordinates for when no coodinates available or no module connected!!
  latitude = 51.6763;
  longitude = 8.3466;

  // when no GPS Module connected  
  if (!Serial1.available())
  {
      dtostrf(latitude, 8, 4, latStr); // converitng float to string 
      dtostrf(longitude, 8, 4, lonStr); // converitng float to string 
      int len = snprintf(buf, sizeof(buf), "{\"latitude\": %s, \"longitude\": %s}", latStr, lonStr);
    
      if (len >= 0 && len < sizeof(buf)){
          client.publish(topic2, buf);
          return 0;
      } else {
          printf("Snprintf error occurred.\n");
          return -1;
      }
  }

  while (Serial1.available() > 0)
    {
      long c = Serial1.read();
        
        //Serial.write(c);

        if (GPS.encode(c)){

          if (!GPS.location.isValid()) {
              //Serial.println("Invalid Fix: Publishing fixed values");
              latitude = 51.6763;
              longitude = 8.3466; 
          }          
          else{
              Serial.println("Valid fix");
              Serial.println();
              latitude = GPS.location.lat();
              longitude = GPS.location.lng(); 
              Serial.print("Lat: ");
              Serial.print(latitude);
              Serial.print(" Lon: ");
              Serial.println(longitude);
          }

            dtostrf(latitude, 8, 4, latStr); // converitng float to string 
            dtostrf(longitude, 8, 4, lonStr); // converitng float to string 
            int len = snprintf(buf, sizeof(buf), "{\"latitude\": %s, \"longitude\": %s}", latStr, lonStr);
            
            if (len >= 0 && len < sizeof(buf)){
                  client.publish(topic2, buf);
                  return 0;
              } else {
                  printf("Snprintf error occurred.\n");

                return -1;
              }
        }  
    }
}


int publish_accelaration(float Acc){

  char accelerationStr[128]; 
  if (Acc < -100){  
    Serial.println("Invalid Acceleration!");
    return -1;
  }
  dtostrf(Acc, 8, 2, accelerationStr); // converitng float to string 
  client.publish(topic, accelerationStr);
  return 0;
}

bool detectAccident(float current_Acc){

  if(current_Acc - initialAcc > THRESHOLD) return true;
  else return false;
}


void setup() {

  Serial.begin(9600);
  Serial1.begin(9600);

  WiFi.begin(ssid, password); // Connect to Wi-Fi
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }
  Serial.println("Connected to WiFi!");

  if (!IMU.begin()) {
    Serial.println("Failed to initialize IMU!");
  }

  client.setServer(mqttServer, mqttPort);   // setting mqtt broker

  // Connect to MQTT broker
  while (!client.connected()) {
    if (client.connect(CAR_ID"EDR_client")) {
      Serial.println("Connected to MQTT broker!");
      client.subscribe(topic);
      client.subscribe(topic2);
      client.subscribe (info_topic);
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

  float currentAcceleration = get_acceleration();
  ret = publish_accelaration(currentAcceleration);
  bool accident_Status = detectAccident(currentAcceleration);

  if(accident_Status){
    Serial.println("Crash Detected!!");
    client.publish (info_topic, CAR_ID);
  }
  if (ret < 0) Serial.println("ERROR: Couldn't publish accelaration.");

  ret = get_GPS();
  if (ret < 0) Serial.println("ERROR: Couldn't publish GPS data.");

  delay(100);
}


