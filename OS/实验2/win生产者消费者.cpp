#include <stdio.h>  
#include <process.h>  
#include <windows.h>  
#define Max 20
//生产者最多在缓冲区中加入15个产品
static int Count_P = 0;//用于记录生产者将产品加入缓冲区的次数
static int Count_C = 0;//用于记录消费者获取的资源个数
static int bufferNum_P = 0;//生产者的缓冲区编号
static int bufferNum_C = 0;//消费者的缓冲区编号
//信号量与关键段  
HANDLE Empty, Full, Mutex;
//生产者 
unsigned int __stdcall ProducerThread(PVOID pM)
{ 
	while (Count_P<Max)
	{
		printf("生产者生产产品\n");
		//等待有空的缓冲区出现  
		WaitForSingleObject(Empty, INFINITE);
		//互斥地访问缓冲区  
		WaitForSingleObject(Mutex,INFINITE);
		Count_P++;
		printf("生产者将%d号产品放入%d号缓冲区中\n",Count_P, bufferNum_P+1);
		bufferNum_P = (bufferNum_P + 1) % 4;//保证缓冲区的编号循环
		//释放信号量
		ReleaseSemaphore(Mutex, 1, NULL);
		ReleaseSemaphore(Full, 1, NULL);
		Sleep(10);
	}
	printf("生产者完成任务，线程结束运行\n");
	return 0;
}
//消费者 
unsigned int __stdcall ConsumerThread(PVOID pM)
{
	while (1)
	{
		//等待非空的缓冲区出现  
		WaitForSingleObject(Full, INFINITE);
		WaitForSingleObject(Mutex, INFINITE);
		//互斥地访问缓冲区  
		if (Count_C < Max) {//需要保证对Count_C的访问也互斥
			Count_C++;
			printf("消费者从%d号缓冲区中取出%d号产品\n", bufferNum_C + 1, Count_C);
			bufferNum_C = (bufferNum_C + 1) % 4;//保证缓冲区编号循环
			ReleaseSemaphore(Mutex, 1, NULL);
			ReleaseSemaphore(Empty, 1, NULL);
			Sleep(60);
		}
		else
			break;
	}
	return 0;
}
int main()
{
	//初始化信号量，缓冲区大小为4
	Empty = CreateSemaphore(NULL, 4, 4, NULL);
	Full = CreateSemaphore(NULL, 0, 4, NULL);
	Mutex = CreateSemaphore(NULL, 1, 1, NULL);
	//生产者1个，消费者3个
	HANDLE hThread[4];
	hThread[0] = (HANDLE)_beginthreadex(NULL, 0, ProducerThread, NULL, 0, NULL);
	hThread[1] = (HANDLE)_beginthreadex(NULL, 0, ConsumerThread, NULL, 0, NULL);
	hThread[2] = (HANDLE)_beginthreadex(NULL, 0, ConsumerThread, NULL, 0, NULL);
	hThread[3] = (HANDLE)_beginthreadex(NULL, 0, ConsumerThread, NULL, 0, NULL);

	WaitForMultipleObjects(4, hThread, TRUE, INFINITE);//主线程等待所有生产者和消费者运行完
	for (int i = 0; i < 4; i++)
		CloseHandle(hThread[i]);

	//销毁信号量
	CloseHandle(Empty);
	CloseHandle(Full);
	return 0;
}