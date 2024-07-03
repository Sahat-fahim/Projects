import time
import random
import pandas as pd
import numpy as np
from sklearn.linear_model import LogisticRegression
import pickle
import numpy as np
import paho.mqtt.client as mqtt


truck_data = pd.read_csv("test.csv")

with open('logistic_regression_model.pkl', 'rb') as file:
    model = pickle.load(file)

probs = model.predict_proba(truck_data)

probs_percent = probs * 100
leading_probability = float('-inf')
Truck_num = 0
index = 0

for row in probs_percent:
    if row[1] > leading_probability:
        leading_probability = row[1]
        Truck_num = index+1
    index += 1


print(f'Truck num: {Truck_num}, Leading Probability: {leading_probability}')




broker_address = "localhost" 
broker_port = 1883 

def on_connect(client, userdata, flags, rc):
    if rc == 0:
        print("Connected to MQTT broker")
    else:
        print("Connection failed")

client = mqtt.Client()
client.on_connect = on_connect
client.connect(broker_address, broker_port, 60)
client.publish("leader", str(Truck_num))
time.sleep(2)

follower_num = random.choice([1, 2, 3])
while follower_num == Truck_num:  # Ensure follower number is different from the predicted number
    follower_num = random.choice([1, 2, 3])

client.publish("follower", str(follower_num))
client.disconnect()
