import csv
import random


num_samples = 5000


data = [['years_of_experience', 'aerodynamics_score', 'technological_score']]
for i in range(num_samples):
    years_of_experience = random.randint(int(0), int(20))
    aerodynamics_score = random.randint(int(5), int(50))
    technological_score = random.randint(1, 10)
    #label = random.choice(['Class A', 'Class B'])
    data.append([years_of_experience, aerodynamics_score, technological_score])

file_path = 'Truck_data.csv'


with open(file_path, 'w', newline='') as file:
    writer = csv.writer(file)
    writer.writerows(data)

print(f"CSV file '{file_path}' generated successfully.")
