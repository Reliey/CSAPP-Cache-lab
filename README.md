# Part 1 Cache模拟器设计

在csim.c提供的程序框架中，编写实现一个Cache模拟器。

###### 1、实现功能

输入：内存访问轨迹。 操作：模拟缓存相对内存访问轨迹的命中（hit）/缺失行为（miss）。 输出：命中、缺失和（缓存行）脱胎/驱除（基于LRU算法）的总数。 完成csim.c文件的结果能够使用命令行参数产生下面的输出结果。 输出形式如下： linux>` ./csim -s 4 -E 2 -b 4 -t traces/yi.trace` 输出：hits:4 misses:5 evictions:2 即当输入时，你完成的csim.c输出和上述相同的功能(*.trace文件下面有详细信息)。

###### 2、要求

（1）模拟器必须在输入参数s,E,b设置为任意值时均能正确工作——即需要使用malloc函数（而不是固定大小值）来为模拟器中的数据结构分配存储空间。 （2）本实验仅关心数据Cache的性能，因此模拟器应忽略所有指令cache访问（即轨迹中“I”起始的行）。 （3）假设内存访问的地址总是正确对齐的，即一次内存访问从不跨越块的边界——因此可忽略访问轨迹中给出的访问请求大小。 （4）必须在main函数最后调用printSummary函数，并如下传之以命中hit、缺失miss和淘汰/驱逐eviction的总数作为参数： `printSummary(hit_count, miss_count, eviction_count);`



# Part 2 优化矩阵转置

##### 1、任务：

①在trans.c中使用C语言编写一个实现矩阵转置的函数transpose_submit。即对于给定的矩阵`Am×n`，得到矩阵`Bn×m`，使得对于任意`0<=i<n、0<=j<m,有B[j][i]=A[i][j]`，其并且使函数调用过程中对cache的不命中数miss尽可能少。

②在如下函数里面编写最终代码： 在文件中给出了

```cpp
char transpose_submit_desc[] = "Transpose submission";
//A是NxM的矩阵，B是MxN的矩阵
void transpose_submit(int M, int N, int A[N][M], int B[M][N])
{   
	//编写矩阵转置代码，把A转置成B
}
void registerFunctions()
{    
	registerTransFunction(transpose_submit, transpose_submit_desc); //注册你的代码，自动测试test-trans.c会调用这个函数
}
```

##### 2、要求：

- 限制对栈的引用——在转置函数中最多定义和使用12个int类型的局部变量，同时不能使用任何long类型的变量或其他位模式数据以在一个变量中存储多个值。原因：实验测试代码不能/不应计数栈的引用访问，而应将注意力集中在对源和目的矩阵的访问模式上。
- 不允许使用递归。如果定义和调用辅助函数，在任意时刻，从转置函数的栈帧到辅助函数的栈帧之间最多可以同时存在12个局部变量。例如，如果转置函数定义了8个局部变量，其中调用了一个使用4个局部变量的函数，而其进一步调用了一个使用2个局部变量的函数，则栈上总共将有14个变量，则违反了本规则。
- 转置函数不允许改变矩阵A，但可以任意操作矩阵B。
- 不允许在代码中定义任何矩阵或使用malloc及其变种。