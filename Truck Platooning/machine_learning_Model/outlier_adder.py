import pandas as pd

# this program is executed twice to add 400 ouliers first 200 0s changes to 1 and then another 200 0s to 1s

data = pd.read_csv('Truck_data_Labeled_random_leaders.csv')
non_leader_indices = data[data['Leader'] == 1].index
num_random_leaders = 200 

random_leader_indices = np.random.choice(non_leader_indices, num_random_leaders, replace=False)
data.loc[random_leader_indices, 'Leader'] = 0

data.to_csv('Truck_data_Labeled_random_leaders.csv', index=False)
