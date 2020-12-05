import matplotlib.pyplot as plt
import math

data = []
for i in range(4):
    with open(f'points_{i}.txt') as f:
        t = f.readlines()
        data.append(([int(x) for x in t[1].replace('\n','').strip().split(' ')],[float(x) for x in t[2].replace('\n','').strip().split(' ')]))
fd0 = [x*x*(data[0][1][1]/(110.0*110.0)) for x in data[0][0]]
fd1 = [x*math.log(x)*(data[2][1][10]/(1010.0*math.log(1010.0))) for x in data[2][0]]
fig, ax = plt.subplots()
l0, = ax.plot(data[0][0], data[0][1], '-o')
l1, = ax.plot(data[1][0], data[1][1], '-o')
l2, = ax.plot(data[2][0], data[2][1], '-o')
l3, = ax.plot(data[3][0][:88], data[3][1], '-o')
f0, = ax.plot(data[0][0], fd0, '-')
# f1, = ax.plot(data[2][0], fd1, '-')
ax.legend((l0, l1, l2), ('Vanilla', 'DAC', 'ImprovedDAC'), loc='upper left')
ax.set_xlabel('len')
ax.set_ylabel('time')
ax.set_title('result')
# plt.show()
plt.savefig('result.png', bbox_inches='tight')