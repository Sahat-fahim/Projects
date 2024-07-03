import pygame
import time
from pygame.event import Event
import random
#import nbimporter
#import leader_selection_Model




trucks = [
    {"x": 650, "y": 10, "dx": 0, "dy": 0, "target_truck": None, "color": "RED"},
    {"x": 650, "y": 10, "dx": 0, "dy": 0, "target_truck": None, "color": "GREEN"},
    {"x": 650, "y": 10, "dx": 0, "dy": 0, "target_truck": None, "color": "BLUE"}
]

leader = int(input("Select a leader (RED: 1, GREEN: 2, or BLUE: 3): ")) 

# Swap the chosen truck color to index 0
trucks[0], trucks[leader - 1] = trucks[leader - 1], trucks[0]

# Shuffle the remaining trucks
random.shuffle(trucks[1:])


pygame.init()

# Set up the window
window_width = 800
window_height = 600
window = pygame.display.set_mode((window_width, window_height))
pygame.display.set_caption("Truck Platooning")

# Set up the colors
RED = (255, 0, 0)
GREEN = (0, 255, 0)
BLUE = (0, 0, 255)
WHITE = (255, 255, 255)

# Set up the truck dimensions
truck_width = 50
truck_height = 50

# Calculate the distance between trucks
truck_distance = truck_width + 10

# Calculate the lane position
lane_x = 80
lane_y = window_height - 530
lane2_x = lane_x + 80
lane2_y = lane_y + 70
lane3_x = 20
lane3_y = window_height

# Run the game loop
running = True
turn = False
turn_complete = False
clock = pygame.time.Clock()
counter = 0
counter_2 = 0



while running:
    for event in pygame.event.get():
        if event.type == pygame.QUIT:
            running = False

       # Control the lead truck with keyboard inputs
        if event.type == pygame.KEYDOWN:
            if event.key == pygame.K_LEFT:
                trucks[0]["dx"] = -1
            elif event.key == pygame.K_RIGHT:
                trucks[0]["dx"] = 1
            elif event.key == pygame.K_UP:
                trucks[0]["dy"] = -1
            elif event.key == pygame.K_DOWN:
                trucks[0]["dy"] = 1
            elif event.key == pygame.K_t:
                turn = not turn

        
        #stop running around when now key is pressed
        # elif event.type == pygame.KEYUP:
        #     if event.key == pygame.K_LEFT or event.key == pygame.K_RIGHT:
        #         trucks[0]["dx"] = 0
        #     elif event.key == pygame.K_UP or event.key == pygame.K_DOWN:
        #         trucks[0]["dy"] = 0

    if turn == False:

        for i, truck in enumerate(trucks):
            truck["x"] += truck["dx"]
            truck["y"] += truck["dy"]
            #print(f'Truck X: {truck["x"]}, Truck dx: {truck["dx"]}')

            if i > 0:
                target_truck = trucks[i-1]  # Get the preceding truck
                if turn_complete:  
                    target_x = target_truck["x"]
                    target_y = target_truck["y"] - truck_distance  # Calculate the target y position
                else: 
                    target_x = target_truck["x"] + truck_distance # Calculate the target x position
                    target_y = target_truck["y"]
                    
                truck["target_truck"] = (target_x, target_y)  # Set the target position
                dx = target_x - truck["x"]
                dy = target_y - truck["y"]

                # Adjust the velocities to move towards the target position
                truck["dx"] = dx / 10
                truck["dy"] = dy / 10
    else: 

        trucks[0]["x"] += trucks[0]["dx"]
        trucks[0]["y"] += trucks[0]["dy"]

        for i, truck in enumerate(trucks):
            if i == 1:
                counter += 1
                if counter >= 500:
                    target_truck = trucks[i-1]  
                    truck["x"] = target_truck["x"] 
                    truck["y"] = target_truck["y"] - truck_distance 
                    truck["target_truck"] = (target_x, target_y)  
                    counter = 0
            if i == 2:
                counter_2 += 1
                if counter_2 >= 1000:
                    target_truck = trucks[i-1]  
                    truck["x"] = target_truck["x"] 
                    truck["y"] = target_truck["y"] - truck_distance 
                    truck["target_truck"] = (target_x, target_y)  
                    counter_2 = 0
                    turn_complete = True
                
    


            
    # Draw the trucks on the window
    window.fill((0, 0, 0))  # Clear the window

    # Draw the lane lines
    pygame.draw.line(window, WHITE, (lane_x, lane_y), (window_width, lane_y), 6)
    pygame.draw.line(window, WHITE, (lane2_x, lane2_y), (window_width, lane2_y), 6)
    pygame.draw.line(window, WHITE, (lane_x, lane_y), (lane_x, window_height), 6)
    pygame.draw.line(window, WHITE, (lane2_x, lane2_y), (lane2_x, window_height), 6)


    for truck in trucks:
        pygame.draw.rect(window, truck["color"], (truck["x"], truck["y"], truck_width, truck_height))

    pygame.display.flip()  # Update the window display
    clock.tick(90)  # Limit the frame rate

pygame.quit()
