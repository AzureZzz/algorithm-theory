import matplotlib.pyplot as plt
import math

flag = 0

fig, ax = plt.subplots()
if flag == 0:
    ## 变化趋势数据
    data = []
    for i in range(0,3):
        with open(f'test1/points_{i}.txt') as f:
            t = f.readlines()
            data.append([[int(x) for x in t[1].replace('\n', '').strip().split(' ')], 
                        [float(x) for x in t[2].replace('\n', '').strip().split(' ')]])
    ## 复杂度曲线
    fd0 = [x*x*(data[0][1][1]/(110.0*110.0)) for x in data[0][0]]
    fd1 = [math.pow(x,1.585)*(data[2][1][1]/(math.pow(110,1.585)))
        for x in data[2][0]]

    l0, = ax.plot(data[0][0], data[0][1], '-o')
    l1, = ax.plot(data[1][0], data[1][1], '-o')
    l2, = ax.plot(data[2][0], data[2][1], '-o')
    f0, = ax.plot(data[0][0], fd0, '-')
    f1, = ax.plot(data[2][0], fd1, '-')

    ax.set_xlabel('large number length')
    ax.set_ylabel('time(ms)')
    ax.set_title('result')

    ax.legend((l0, l1, l2,f0,f1), ('Vanilla', 'DAC', 'ImprovedDAC','T*n^2','T*n^1.585'), loc='upper left')
    plt.show()
    # plt.savefig('test1/result1.png', bbox_inches='tight')


if flag ==1:
    # 表格散点图数据
    data = [[[10, 100, 1000, 10000, 100000, 1000000], [0.0007691, 0.041211, 3.5319, 350.5247, 35333.5209, 3410786.916]],
             [[10, 100, 1000, 10000, 100000, 1000000], [0.0008739, 0.042144, 4.6138, 418.6627, 40056.0319, 4455187.463]],
             [[10, 100, 1000, 10000, 100000, 1000000, 10000000], [0.0008367, 0.041733, 1.7158, 69.1501, 2672.1968, 103587.818, 3987544.697]]]
             
    b = math.log10(0.04/10000.0)
    b1 = math.log10(0.0008/100.0)+(0.5)*math.log10(10)
    fd00 = [math.log10(x) for x in data[0][0]]
    fd01 = [2*math.log10(x)+b for x in data[0][0]]
    fd10 = [math.log10(x) for x in data[2][0]]
    fd11 = [1.585*math.log10(x) + b1 for x in data[2][0]]

    data[0][0] = [math.log10(x) for x in data[0][0]]
    data[0][1] = [math.log10(x) for x in data[0][1]]
    data[1][0] = [math.log10(x) for x in data[1][0]]
    data[1][1] = [math.log10(x) for x in data[1][1]]
    data[2][0] = [math.log10(x) for x in data[2][0]]
    data[2][1] = [math.log10(x) for x in data[2][1]]

    ax.set_xlabel('large number length log10(n)')
    ax.set_ylabel('time log10(n)')
    ax.set_title('result')
    l0, = ax.plot(data[0][0], data[0][1], '^')
    l1, = ax.plot(data[1][0], data[1][1], 'o')
    l2, = ax.plot(data[2][0], data[2][1], 'v')
    f0, = ax.plot(fd00, fd01, '-')
    f1, = ax.plot(fd10, fd11, '-')

    ax.legend((l0, l1, l2, f0, f1), ('Vanilla','DAC','ImprovedDAC','2log10(n)+b1','1.585log10(n)+b2'), loc='upper left')
    plt.show()
    # plt.savefig('test1/result2.png', bbox_inches='tight')


## 查看threshold的影响
if flag == 2:
    data = []
    for i in range(6,7):
        with open(f'test1/points_{i}.txt') as f:
            t = f.readlines()
            data.append([[int(x) for x in t[1].replace('\n', '').strip().split(' ')], 
                        [float(x) for x in t[2].replace('\n', '').strip().split(' ')]])
    ## 复杂度曲线

    l0, = ax.plot(data[0][0], data[0][1], '-')

    ax.set_xlabel('threshold')
    ax.set_ylabel('time')
    ax.set_title('result')

    ax.legend((l0,), ('number length=1000',), loc='upper right')
    plt.show()
    # plt.savefig('test1/result3.png', bbox_inches='tight')

