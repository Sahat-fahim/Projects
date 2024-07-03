import paho.mqtt.client as mqtt
import time



broker_address = "localhost"
broker_port = 1883   
leader = False
follower_1 = False
follower_2 = False
state = ""
lane_change = False

def on_connect(client, userdata, flags, rc):
    if rc == 0:
        print("Connected to the Operator!!")
        print("Waiting for leader to be selected...")
        client.subscribe("leader")
        client.subscribe("commands")
        client.subscribe("follower")
    else:
        print("Connection failed")


def on_message(client, userdata, msg):
    global state, leader, follower_1, follower_2
    topic = msg.topic

    if topic == "leader":
        if msg.payload.decode() == "1": 
            print("I am the leader")
            leader = True
            state = "driving"
    
    if topic == "commands":
        state = msg.payload.decode()
    
    if leader is False:
        if topic == "follower":
            if msg.payload.decode() == "1":
                print("I am the follower 1")
                follower_1 = True
            else:
                print("I am the follower 2")
                follower_2 = True

    topic = ''

client = mqtt.Client()
client.on_connect = on_connect
client.on_message = on_message


client.connect(broker_address, broker_port, 60)
client.loop_start()

def leading_operations():
    global lane_change, state

    if (state == "driving"):
        time.sleep(5)
        client.publish("commands", "driving")
        print(f'Leader: {state}')
        time.sleep(5)

        if lane_change: 
            state = "turn"
            lane_change = False
        else: 
            state = "lane_change"

    elif (state == "lane_change"):
        print(f'Leader: {state}')
        client.publish("commands", "lane_change") 
        time.sleep(4)       
        state = "driving"
        lane_change = True

    elif (state == "turn"):
        print(f'Leader: {state}')
        client.publish("commands", "turn")
        time.sleep(7)       
        state = "driving"


def follower_1_operations():
    global state

    if (state == "driving"):
        print("Follower 1: driving")
        state = ''
    elif (state == "lane_change"):
        
        time.sleep(1)
        print(f'Follower 1: lane_change')
        state = ''    

    elif (state == "turn"):
        time.sleep(3)
        print(f'Follower 1: turn')
        state = ''
        

def follower_2_operations():
    global state
    if (state == "driving"):
        print("Follower 2: driving")
        state = ''

    elif (state == "lane_change"):
        time.sleep(2)
        print(f'Follower 2: lane_change')
        state = ''

    elif (state == "turn"):
        time.sleep(6)
        print(f'Follower 2: turn')
        state = ''


try:
    while True:
        if leader: 
            leading_operations()
        elif follower_1:
            follower_1_operations()
        elif follower_2:
            follower_2_operations()

except KeyboardInterrupt:
    pass
    
client.disconnect()
client.loop_stop()
