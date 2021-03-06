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

**Vanilla**

Algorithmic complexity：$$ O(n^2) $$

**Divide and Conquer**
$$
\begin{aligned}
Input:&X,Y\\
Compute:&Z=X*Y\\
Divide:&X=A*10^n+B,Y=C*10^n+D\\\\
Z=&X*Y\\
=&(A*10^n+B)*(C*10^n+D)\\
=&AC*10^{2n}+(AD+BC)*10^n+BD\ \ \ \ \ \ \ \ \ \ \ \ \ \ (1)
\end{aligned}
$$
Algorithmic complexity：$$ O(n^2) $$



**Improved  Divide and conquer**
$$
\begin{aligned}
Z=&AC*10^{2n}+(AD+BC)*10^n+BD\\
=&AC*10^{2n}+[(A+B)(C+D)-AC-BD]*10^n+BD\ \ \ \ \ \ \ \ \ \ \ \ \ \ (2)
\end{aligned}
$$
By converting formula (1) to formula (2), one multiplication is eliminated.

Algorithmic complexity：O(n^1.585)



### 3.Experimental results 

Runtime environments:

> - **CPU**：Intel(R) Core(TM) i7-9750H CPU @ 2.60 GHz x 12  cores：6
> - **RAM**：2*8G, DDR4 2667Mhz 64bit
> - **Disk**：1TB SSD PCI-E
> - **OS**：Windows 10 (x64)
> - **Compiler**：g++(tdm-1) 5.1.0
> - **IDE**：Visual Studio Code



| Length of large Number | Vanilla (ms) |  DAC (ms)   | Imporved DAC (ms) |
| :--------------------: | :----------: | :---------: | :---------------: |
|           10           |  0.0007691   |  0.0008739  |     0.0008367     |
|          100           |   0.041211   |  0.042144   |     0.041733      |
|         1,000          |    3.5319    |   4.6138    |      1.7158       |
|         10,000         |   350.5247   |  418.6627   |      69.1501      |
|        100,000         |  35333.5209  | 40056.0319  |     2672.1968     |
|       1,000,000        | 3410786.916  | 4455187.463 |    103587.818     |
|       10,000,000       |      -       |      -      |    3987544.697    |
|      100,000,000       |      -       |      -      |   153360969.04    |




## Test2



### 1.Main work

Problem description：

https://github.com/AzureZzz/algorithm-theory/blob/master/test2/Skyline.docx

or

https://github.com/AzureZzz/algorithm-theory/blob/master/test2/RDF-skyline.pdf





### 2.Data

Used data：*Yago*，*Yago_small*.

The datasets can be download by link: https://pan.baidu.com/s/103ohzftpLh284ZZ--wNufg  password: fw5m

The data contains the following three files：

- **edge.txt**

> It stores a linked list of directed graph adjacencies, with each row representing a vertex and its adjacent vertex.
>
> eg：8:6291031,5330605,6481451,6280292, which means vertex 8 have four adjacent vertices, and they are 6291031,5330605,6481451,6280292.

- **node_keywords.txt**

> It holds the text properties of the vertices, and each row represents the text properties of one vertex.
>
> eg： 0:8973992,10029808,8435980, which means vertex 0 have three keywords, and they are 8973992,10029808,8435980.

- **placeid2coordYagoVB.txt**

> It contains **4,774,796 ** vertices representing sites and their spatial coordinates.
>
> eg：0: 33.84833 35.58278, which means vertex 0 is a site and its spatial coordinates are 33.84833 35.58278.

*Yago* is a large scale graph, which contains **8,091,179** vertices and **50,415,307** edges.

*Yago_small* is a small scale graph, which contains **95,883** edges.





### 3.Methods

**B-tree**

reference：https://www.cnblogs.com/lianzhilei/p/11250589.html



**algorithm flow chart**:

```flow
st=>start: start
build=>operation: read data and build graph
input=>inputoutput: input keywords
search=>operation: 1.search keyword
cal=>operation: 2.calculate shortest distance
select=>operation: 3.select SP
output=>inputoutput: output SP
e=>end: end

st->build->input->search->cal->select->output->e

```








### 4.Experimental results 

Runtime environments:

> - **CPU**：Intel(R) Xeon(R) Silver 4110 CPU @ 2.10GHz × 32  cores：8
> - **RAM**：2*32G, DDR4 2400Mhz 64bit
> - **Disk**：512GB SSD PCI-E
> - **OS**：Ubuntu 20.04 STL (x64)
> - **Compiler**：g++ 9.3.0
> - **Debug**：GUN gdb 9.2
> - **IDE**：Visual Studio Code

TODO



