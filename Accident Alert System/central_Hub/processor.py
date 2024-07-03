import paho.mqtt.client as mqtt
import json
import time



broker_address = "192.168.103.137"  
broker_port = 1883  

info_topic = "info_Car"
car_ID = "Car_X"
imu_topic = car_ID+"/EDR/IMU_Data"
gps_topic = car_ID+"/EDR/GPS_Data"


accelaration = None
latitude = None
longitude = None

def publish_location(lat, lon):

    location_data = {
        "latitude": lat,
        "longitude": lon
    }
    payload = json.dumps(location_data)
    client.publish("Hospital/rescue_console", payload)

# MQTT callback
def on_message(client, userdata, message):
    
    global accelaration, latitude, longitude, car_ID, imu_topic, gps_topic
    
    topic = message.topic
    payload = message.payload.decode()

    if topic == gps_topic:
        data = json.loads(payload)
        latitude = data["latitude"]
        longitude = data["longitude"]
        print(f"Latitude: {latitude}, Longitude: {longitude}")
        publish_location(lat=latitude, lon=longitude)
        gps_topic = ""

    elif topic == imu_topic:
        print(f"Accelaration: {payload}")
        accelaration = payload
        imu_topic = ""

    elif topic == info_topic: 
        
        print(f"info: {payload}")
        car_ID = payload
        imu_topic = car_ID+"/EDR/IMU_Data"
        gps_topic = car_ID+"/EDR/GPS_Data"
        print(f"info: {imu_topic}")
        client.subscribe(gps_topic)
        client.subscribe(imu_topic)



    


client = mqtt.Client()
client.on_message = on_message
client.connect(broker_address, broker_port)

# client.subscribe(gps_topic)
# client.subscribe(imu_topic)
client.subscribe(info_topic)
client.loop_forever()
time.sleep(1)


