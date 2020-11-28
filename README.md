# Algorithm theory experiments.


This project mainly consists of two experiments：

- Test1: Large integer multiplication.
- Test2: Semantic location SKYLINE queries based on RDF graphs.


## Test1

### 1.Main work

Three algorithms of large integer multiplication are implemented in C++ programming language, including:
- Vanilla multiply.
- Multiply based on Divide and conquer.
- Multiply based on Improved  Divide and conquer.

### 2.Methods

TODO



### 4.Experimental results 

Runtime environments:

- CPU：Intel(R) Core(TM) i7-9750H CPU @ 2.60 GHz x 12  cores：6
- RAM：2*8G, DDR4 2667Mhz 64bit
- Disk：1TB SSD PCI-E
- OS：Windows 10 (x64)



| Length of large Number | Vanilla (ms) | DAC (ms) | Imporved DAC (ms) |
| :--------------------: | :----------: | :------: | :---------------: |
|           10           |    0.0025    |  0.0033  |      0.0023       |
|          100           |    0.1401    |  0.1234  |      0.1617       |
|          1000          |   12.2785    | 13.4647  |      5.0968       |
|         10000          |   1078.64    | 1050.59  |      166.827      |
|         100000         |   98789.7    |  102907  |      6370.87      |
|        1000000         |   10190900   | 10302800 |      221665       |
|        10000000        |      -       |    -     |      8873400      |




## Test2

### 1.Main work

TODO

### 2.Dataset

Used datasets：Yago，Yago_small.

The datasets can be download by link: https://pan.baidu.com/s/103ohzftpLh284ZZ--wNufg  password: fw5m

### 3.Methods

TODO

### 4.Experimental results 

Runtime environments:

- CPU：Intel(R) Xeon(R) Silver 4110 CPU @ 2.10GHz × 32  cores：8
- RAM：2*32G, DDR4 2400Mhz 64bit
- Disk：512GB SSD PCI-E
- OS：Ubuntu 20.04 STL (x64)

TODO
