import matplotlib.pyplot as plt
import math

# data = []
# for i in range(4):
#     with open(f'test1/points_{i}.txt') as f:
#         t = f.readlines()
#         data.append([[int(x) for x in t[1].replace('\n', '').strip().split(' ')], 
#                     [float(x) for x in t[2].replace('\n', '').strip().split(' ')]])
# fd0 = [x*x*(data[0][1][1]/(110.0*110.0)) for x in data[0][0]]
# fd1 = [x*math.log(x)*(data[2][1][10]/(1010.0*math.log(1010.0)))
#        for x in data[2][0]]

data = [[[10, 100, 1000, 10000, 100000, 1000000], [0.001424, 0.106247, 10.216012, 993.97512, 98789.6876, 10190900.0]],
         [[10, 100, 1000, 10000, 100000, 1000000], [0.0033, 0.1234, 12.4647, 1050.59, 102907.0, 10302800.0]],
         [[10, 100, 1000, 10000, 100000, 1000000, 10000000], [0.001591, 0.098311, 4.82585, 161.70933, 5841.69055, 221665.0, 8873400.0]]]
data[0][0] = [math.log10(x) for x in data[0][0]]
data[0][1] = [math.log10(x) for x in data[0][1]]
data[1][0] = [math.log10(x) for x in data[1][0]]
data[1][1] = [math.log10(x) for x in data[1][1]]
data[2][0] = [math.log10(x) for x in data[2][0]]
data[2][1] = [math.log10(x) for x in data[2][1]]


fig, ax = plt.subplots()
l0, = ax.plot(data[0][0], data[0][1], '-^')
l1, = ax.plot(data[1][0], data[1][1], '-v')
l2, = ax.plot(data[2][0], data[2][1], '-o')
# l3, = ax.plot(data[3][0][:88], data[3][1], '-o')
# f0, = ax.plot(data[0][0], fd0, '-')
# f1, = ax.plot(data[2][0], fd1, '-')

ax.set_xlabel('len log10(n)')
ax.set_ylabel('time log10(n)')
ax.set_title('result')
# ax.set_xlabel('len')
# ax.set_ylabel('time')
# ax.set_title('result')

ax.legend((l0, l1, l2), ('Vanilla', 'DAC', 'ImprovedDAC'), loc='upper left')
plt.show()
# plt.savefig('test1/result1.png', bbox_inches='tight')
