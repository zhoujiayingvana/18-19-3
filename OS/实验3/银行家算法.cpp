#include <iostream>
#include <stdio.h>
#include <Windows.h>
#include <process.h>
#include<time.h>
using namespace std;
const int m = 3;//m为每个进程需要的资源种类，3个
const int n = 5;//n为进程数量，5个
HANDLE mutex1, mutex2;//互斥信号量，mutex1用于进程请求资源，mutex2用于执行安全算法
int available[m] = { 3,3,2 };//available为系统可用资源数
int allocation[n][m] = {//分配资源情况
	{ 0,1,0 },
	{ 2,0,0 },
	{ 3,0,2 },
	{ 2,1,1 },
	{ 0,0,2 }
};
int maximum[n][m] = {//进程需要最大资源情况
	{ 7,5,3 },
	{ 3,2,2 },
	{ 9,2,2 },
	{ 2,2,2 },
	{ 4,3,3 }
};
int need[n][m] = { 0 };//进程需要资源情况
bool finished[n] = { false };//安全算法中判断进程是否使用完资源
int request[n][m] = { 0 };//进程请求资源情况

/**
比较进程请求资源和当前可用资源，判断是否可以分配给该进程资源
**/
bool compare(int request_need[][m], int *available_work, int i) {
	for (int j = 0; j < m; j++) {
		if (request_need[i][j] > available_work[j])
			return false;
	}
	return true;
}

/**
比较进程需要资源和当前可用资源，判断是否可以分配给该进程资源
**/
bool compare(int request[][m], int need[][m], int i) {
	for (int j = 0; j < m; j++) {
		if (request[i][j] > need[i][j])
			return false;
	}
	return true;
}

/**
安全算法
**/
bool safe(int need[][m], int *available, int allocation[][m]) {
	int work[m] = { 0 };
	for (int i = 0; i < m; i++) {
		work[i] = available[i];
	}
	bool finish[n] = { false };
	for (int k = 0; k < n; k++)//产生安全序列
	{
		int i;
		for (i = 0; i < n; i++)//寻找所有符合条件的进程
		{
			if (finish[i] == false && compare(need, work, i)) {
				for (int j = 0; j < m; j++)//该进程可以得到资源执行，执行完毕后释放资源
				{
					work[j] = allocation[i][j] + work[j];
					finish[i] = true;

				}
			}
		}
		i = 0;
	}
	cout << endl;
	int count = 0;
	for (int i = 0; i < n; i++) {
		if (finish[i]) {
			count++;
		}
	}
	if (count == n)//所有进程都可以执行,系统安全
		return true;
	else//不安全
		return false;
}


unsigned int __stdcall process(LPVOID pM) {
	int j = *(int *)pM;
	srand(unsigned(time(0)));
	//j为当前线程序号
	while (!finished[j]) {
		for (int i = 0; i < m; i++) {
			request[j][i] = rand() % 3;
		}
		WaitForSingleObject(mutex1, INFINITE);//进程请求资源
		cout << "Process No." << j << " request A:" << request[j][0]
			<< ", B:" << request[j][1] << ", C:"
			<< request[j][2] << endl;
		ReleaseSemaphore(mutex1, 1, NULL);

		WaitForSingleObject(mutex2, INFINITE);//更新三个矩阵
		if (compare(request, need, j)) {
			if (compare(request, available, j)) {
				for (int i = 0; i < m; i++) {
					available[i] -= request[j][i];
					allocation[j][i] += request[j][i];
					need[j][i] -= request[j][i];
				}
				if (safe(need, available, allocation)) {//拟分配资源判断是否安全
					cout << "Process No." << j << " request completed." << endl;
					finished[j] = true;
					for (int i = 0; i < m; i++) {
						available[i] += allocation[j][i];
						allocation[j][i] = 0;
						need[j][i] = 0;
					}
					cout << "Available:A:" << available[0]
						<< ", B:" << available[1]
						<< ", C:" << available[2] << endl;
				}
				else {//不安全取消拟分配的资源
					for (int i = 0; i < m; i++) {
						available[i] += request[j][i];
						allocation[j][i] -= request[j][i];
						need[j][i] += request[j][i];
					}
					cout << "Error : Not safe." << endl;
					cout << "Process No." << j << " request failed." << endl;
					cout << "Available:A:" << available[0]
						<< ", B:" << available[1]
						<< ", C:" << available[2] << endl;
					cout << endl;
					Sleep(1000);//1s后继续发送请求
				}
			}
			else {
				cout << "Error : Request > Available" << endl;
				cout << "Process No." << j << " request failed." << endl;
				cout << "Available:A:" << available[0]
					<< ", B:" << available[1]
					<< ", C:" << available[2] << endl;
				Sleep(1000);//1s后继续发送请求
			}
		}
		else {
			cout << "Error : Request > Need" << endl;
			cout << "Process No." << j << " request failed." << endl;
			cout << "Available:A:" << available[0]
				<< ", B:" << available[1]
				<< ", C:" << available[2] << "\n" << endl;
			Sleep(1000);//1s后继续发送请求
		}
		ReleaseSemaphore(mutex2, 1, NULL);//释放mutex2
	}
	//该线程2s后完成任务，释放资源
	Sleep(2000);
	cout << "Process " << j << " has finished\n";
	WaitForSingleObject(mutex2, INFINITE);//准备释放资源
	for (int i = 0; i < m; i++) {
		available[i] += allocation[j][i];
	}
	ReleaseSemaphore(mutex2, 1, NULL);
	return 0L;
}

int main(int argc, char *argv[])
{
	for (int i = 0; i < n; i++)
		for (int j = 0; j < m; j++)
			need[i][j] = maximum[i][j] - allocation[i][j];
	cout << "Available:A:" << available[0]
		<< ", B:" << available[1]
		<< ", C:" << available[2] << ".\n" << endl;
	const int num = 10;
	mutex1 = CreateSemaphore(NULL, 1, 1, NULL);
	mutex2 = CreateSemaphore(NULL, 1, 1, NULL);
	HANDLE hThread[5];
	unsigned int a = 0, b = 1, c = 2, d = 3, e = 4;
	hThread[0] = (HANDLE)_beginthreadex(NULL, 0, process, &a, 0, NULL);
	Sleep(30);
	hThread[1] = (HANDLE)_beginthreadex(NULL, 0, process, &b, 0, NULL);
	Sleep(30);
	hThread[2] = (HANDLE)_beginthreadex(NULL, 0, process, &c, 0, NULL);
	Sleep(30);
	hThread[3] = (HANDLE)_beginthreadex(NULL, 0, process, &d, 0, NULL);
	Sleep(30);
	hThread[4] = (HANDLE)_beginthreadex(NULL, 0, process, &e, 0, NULL);
	Sleep(30);
	WaitForMultipleObjects(5, hThread, TRUE, INFINITE);//主线程等待所有生产者和消费者运行完
	for (int i = 0; i < 5; i++)
		CloseHandle(hThread[i]);
	//销毁信号量
	CloseHandle(mutex1);
	CloseHandle(mutex2);
	return 0;
}