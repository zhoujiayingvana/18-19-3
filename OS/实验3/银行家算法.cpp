#include <iostream>
#include <stdio.h>
#include <Windows.h>
#include <process.h>
#include<time.h>
using namespace std;
const int m = 3;//mΪÿ��������Ҫ����Դ���࣬3��
const int n = 5;//nΪ����������5��
HANDLE mutex1, mutex2;//�����ź�����mutex1���ڽ���������Դ��mutex2����ִ�а�ȫ�㷨
int available[m] = { 3,3,2 };//availableΪϵͳ������Դ��
int allocation[n][m] = {//������Դ���
	{ 0,1,0 },
	{ 2,0,0 },
	{ 3,0,2 },
	{ 2,1,1 },
	{ 0,0,2 }
};
int maximum[n][m] = {//������Ҫ�����Դ���
	{ 7,5,3 },
	{ 3,2,2 },
	{ 9,2,2 },
	{ 2,2,2 },
	{ 4,3,3 }
};
int need[n][m] = { 0 };//������Ҫ��Դ���
bool finished[n] = { false };//��ȫ�㷨���жϽ����Ƿ�ʹ������Դ
int request[n][m] = { 0 };//����������Դ���

/**
�ȽϽ���������Դ�͵�ǰ������Դ���ж��Ƿ���Է�����ý�����Դ
**/
bool compare(int request_need[][m], int *available_work, int i) {
	for (int j = 0; j < m; j++) {
		if (request_need[i][j] > available_work[j])
			return false;
	}
	return true;
}

/**
�ȽϽ�����Ҫ��Դ�͵�ǰ������Դ���ж��Ƿ���Է�����ý�����Դ
**/
bool compare(int request[][m], int need[][m], int i) {
	for (int j = 0; j < m; j++) {
		if (request[i][j] > need[i][j])
			return false;
	}
	return true;
}

/**
��ȫ�㷨
**/
bool safe(int need[][m], int *available, int allocation[][m]) {
	int work[m] = { 0 };
	for (int i = 0; i < m; i++) {
		work[i] = available[i];
	}
	bool finish[n] = { false };
	for (int k = 0; k < n; k++)//������ȫ����
	{
		int i;
		for (i = 0; i < n; i++)//Ѱ�����з��������Ľ���
		{
			if (finish[i] == false && compare(need, work, i)) {
				for (int j = 0; j < m; j++)//�ý��̿��Եõ���Դִ�У�ִ����Ϻ��ͷ���Դ
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
	if (count == n)//���н��̶�����ִ��,ϵͳ��ȫ
		return true;
	else//����ȫ
		return false;
}


unsigned int __stdcall process(LPVOID pM) {
	int j = *(int *)pM;
	srand(unsigned(time(0)));
	//jΪ��ǰ�߳����
	while (!finished[j]) {
		for (int i = 0; i < m; i++) {
			request[j][i] = rand() % 3;
		}
		WaitForSingleObject(mutex1, INFINITE);//����������Դ
		cout << "Process No." << j << " request A:" << request[j][0]
			<< ", B:" << request[j][1] << ", C:"
			<< request[j][2] << endl;
		ReleaseSemaphore(mutex1, 1, NULL);

		WaitForSingleObject(mutex2, INFINITE);//������������
		if (compare(request, need, j)) {
			if (compare(request, available, j)) {
				for (int i = 0; i < m; i++) {
					available[i] -= request[j][i];
					allocation[j][i] += request[j][i];
					need[j][i] -= request[j][i];
				}
				if (safe(need, available, allocation)) {//�������Դ�ж��Ƿ�ȫ
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
				else {//����ȫȡ����������Դ
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
					Sleep(1000);//1s�������������
				}
			}
			else {
				cout << "Error : Request > Available" << endl;
				cout << "Process No." << j << " request failed." << endl;
				cout << "Available:A:" << available[0]
					<< ", B:" << available[1]
					<< ", C:" << available[2] << endl;
				Sleep(1000);//1s�������������
			}
		}
		else {
			cout << "Error : Request > Need" << endl;
			cout << "Process No." << j << " request failed." << endl;
			cout << "Available:A:" << available[0]
				<< ", B:" << available[1]
				<< ", C:" << available[2] << "\n" << endl;
			Sleep(1000);//1s�������������
		}
		ReleaseSemaphore(mutex2, 1, NULL);//�ͷ�mutex2
	}
	//���߳�2s����������ͷ���Դ
	Sleep(2000);
	cout << "Process " << j << " has finished\n";
	WaitForSingleObject(mutex2, INFINITE);//׼���ͷ���Դ
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
	WaitForMultipleObjects(5, hThread, TRUE, INFINITE);//���̵߳ȴ����������ߺ�������������
	for (int i = 0; i < 5; i++)
		CloseHandle(hThread[i]);
	//�����ź���
	CloseHandle(mutex1);
	CloseHandle(mutex2);
	return 0;
}