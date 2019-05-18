import matplotlib.pyplot as plt
plt.switch_backend('agg')
import numpy as np

# number of threads
x = np.array([1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16])

# reading data
col_1    = [data.split(' ')[0] for data in open("chunk_default.txt").readlines()]
col_2 = [data.split(' ')[1] for data in open("chunk_default.txt").readlines()]

# converting string to int
init    = [float(x) for x in col_1]
reorder = [float(x) for x in col_2]
total   = [init[i]+reorder[i] for i in range(len(init))]  
total   = np.array(total)

# plotting
plt.plot(x,total)
plt.title('Static')
plt.xlabel('Threads')
plt.ylabel('Speedup')
plt.savefig('Static.png')
