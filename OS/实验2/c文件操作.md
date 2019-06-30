# c文件操作

Linux下GCC编译C语言

1.创建一个.C文件
	# vi/vim  name.c

2.编译name.c文件,生成可执行文件
	# gcc name.c

3.执行文件，在默认情况下产生的可执行程序名为a.out
	# ./a.out

4.通常可以通过gcc的“-o”选项来指定自己的可执行程序名称。
	# gcc -o name name.c
  生成可执行文件 name 执行：
	# ./name


5.使用gcc的“-c”标记来编译支持库代码(对多个文件进行编译)
	# gcc -c name.c
  输出结果为一个名为 name.o 的文件

例子：	# gcc -c name.c
	# gcc -c main.c
	现在有了两个目标文件：name.o和main.o。它们包含能够被Linux执行的目标代码。
再一次调用GCC来执行连接阶段的工作： 
	# gcc -o NameMain name.o main.o
执行调用：./NameMain

命令简化：GCC对如何将多个源文件编译为一个可执行程序有内置的规则。 
	# gcc -o NameMain name.c main.c 
	# ./NameMain


6. 使用外部函数库 
    GCC常常与包含标准例程的外部软件库结合使用，几乎每一个Linux应用程序都依赖于由GNU C函数库GLIBC。	
    编译命令：GCC的"-lm"选项，它告诉GCC查看系统提供的数学库（libm）

  # gcc -o name -lm name.c


7.共享函数库与静态函数库，Linux系统上的函数库分为两种不同的类型：共享的和静态的 。
（1）静态函数库：每次当应用程序和静态连接的函数库一起编译时，任何引用的库函数中的代码都会被直接包含进最终的二进制程序。 

（2）共享函数库：包含每个库函数的单一全局版本，它在所有应用程序之间共享。这一过程背后所涉及的机制相当复杂；
但主要依靠的是现代计算机的虚拟内存能力，它允许包含库函数的物理内存安全地在多个独立用户程序之间共享。 

（3）使用共享函数库不仅减少了文件的容量和Linux应用程序在内存中覆盖的区域，而且它还提高了系统的安全性；
一个被许多不同程序同时调用的共享函数库很可能会驻留在内存中，以在需要使用它时被立即使用，而不是位于磁盘的交换分区中；
这有助于进一步减少一些大型Linux应用程序的装载时间。

 

 

### **新建c文件**

​	touch 文件名.c

### 打开c文件

### **退出c文件**

​	esc

​	wq保存并退出

![1557104316748](C:\Users\温莎公爵\AppData\Roaming\Typora\typora-user-images\1557104316748.png)

# 信号量

## Pthread信号量

信号量分两种，一种是无名信号量，一种是有名信号量。无名信号量一般用于线程间同步或互斥，而有名信号量一般用于进程间同步或互斥

![img](https://images2015.cnblogs.com/blog/1050425/201612/1050425-20161212212117401-1255553625.png)

![img](https://images2015.cnblogs.com/blog/1050425/201612/1050425-20161212212124073-1678149336.png)

**创建信号量：**sem_t 信号量名；

**初始化信号量：**sem_init(**&**name,0,初值)；

**wait：**sem_wait(**&**name);

**signal：**sem_post(**&**name);

## Win32 信号量

**创建信号量：**HANDLE 信号量名；

**初始化信号量：**HANDLE CreateSemaphore( LPSECURITY_ATTRIBUTES lpAttributes,
LONG lInitialCount,
LONG lMaximumCount,
LPCTSTR lpName
); 
参数
lpAttributes  安全属性。如果是NULL 就表示要使用默认属
性。Windows 95 忽略这一参数。
lInitialCountsem aphore 的初值。必须大于或等于0，并且小于
或等于lMaxim umCount。
lMaximumCountSem aphore 的最大值。这也就是在同一时间内能够
锁住sem aphore 之线程的最多个数。
lpNameSem aphore 的名称（一个字符串）。任何线程（或
进程）都可以根据这一名称引用到这个
semaphore。这个值可以是NULL，意思是产生一
个没有名字的sem aphore

```c
HANDLE hSemaphore；
hSemaphore = CreateSemaphore(NULL,3,3,NULL);//创建信号量 ，最大计数值为3
```

**wait：**WaitForSingleObject(__in HANDLE hHandle,

__in DWORD dwMilliseconds

);

参数																						*hHandle*[in]对象句柄。可以指定一系列的对象，如Event、Job、Memory resource notification、Mutex、Process、Semaphore、Thread、Waitable timer等。

*dwMilliseconds*[in]定时时间间隔，单位为milliseconds（毫秒）.如果指定一个非零值，函数处于等待状态直到*hHandle*标记的对象被触发，或者时间到了。如果*dwMilliseconds*为0，对象没有被触发信号，函数不会进入一个等待状态，它总是立即返回。如果*dwMilliseconds*为INFINITE，对象被触发信号后，函数才会返回。

```c
WaitForSingleObject(Name, INFINITE);
```

**signal：**  ReleaseSemaphore(
    in      HANDLE hSemaphore,
    in      LONG lReleaseCount,
    __out_opt LPLONG lpPreviousCount
    );

参数

hSemaphore是要增加的信号量句柄。
lReleaseCount是增加的计数。
lpPreviousCount是否返回前一次的信号量值。  

```c
ReleaseSemaphore(Name, 1, NULL);
```

**main函数：**

```C
//初始化信号量
	Empty = CreateSemaphore(NULL, 4, 4, NULL);
	Full = CreateSemaphore(NULL, 0, 4, NULL);
	Mutex = CreateSemaphore(NULL, 1, 1, NULL);
	//生产者1个，消费者3个
	HANDLE hThread[4];
	hThread[0] = (HANDLE)_beginthreadex(NULL, 0, ProducerThread, NULL, 0, NULL);
	hThread[1] = (HANDLE)_beginthreadex(NULL, 0, ConsumerThread, NULL, 0, NULL);
	hThread[2] = (HANDLE)_beginthreadex(NULL, 0, ConsumerThread, NULL, 0, NULL);
	hThread[3] = (HANDLE)_beginthreadex(NULL, 0, ConsumerThread, NULL, 0, NULL);

	WaitForMultipleObjects(4, hThread, TRUE, INFINITE);//等待所有子线程执行完毕后再执行main函数
	for (int i = 0; i < 4; i++)
		CloseHandle(hThread[i]);

	//销毁信号量
	CloseHandle(Empty);
	CloseHandle(Full);
	return 0;
```

