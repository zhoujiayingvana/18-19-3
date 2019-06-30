#include <stdio.h>  
#include <process.h>  
#include <windows.h>  
#define Max 20
//����������ڻ������м���15����Ʒ
static int Count_P = 0;//���ڼ�¼�����߽���Ʒ���뻺�����Ĵ���
static int Count_C = 0;//���ڼ�¼�����߻�ȡ����Դ����
static int bufferNum_P = 0;//�����ߵĻ��������
static int bufferNum_C = 0;//�����ߵĻ��������
//�ź�����ؼ���  
HANDLE Empty, Full, Mutex;
//������ 
unsigned int __stdcall ProducerThread(PVOID pM)
{ 
	while (Count_P<Max)
	{
		printf("������������Ʒ\n");
		//�ȴ��пյĻ���������  
		WaitForSingleObject(Empty, INFINITE);
		//����ط��ʻ�����  
		WaitForSingleObject(Mutex,INFINITE);
		Count_P++;
		printf("�����߽�%d�Ų�Ʒ����%d�Ż�������\n",Count_P, bufferNum_P+1);
		bufferNum_P = (bufferNum_P + 1) % 4;//��֤�������ı��ѭ��
		//�ͷ��ź���
		ReleaseSemaphore(Mutex, 1, NULL);
		ReleaseSemaphore(Full, 1, NULL);
		Sleep(10);
	}
	printf("��������������߳̽�������\n");
	return 0;
}
//������ 
unsigned int __stdcall ConsumerThread(PVOID pM)
{
	while (1)
	{
		//�ȴ��ǿյĻ���������  
		WaitForSingleObject(Full, INFINITE);
		WaitForSingleObject(Mutex, INFINITE);
		//����ط��ʻ�����  
		if (Count_C < Max) {//��Ҫ��֤��Count_C�ķ���Ҳ����
			Count_C++;
			printf("�����ߴ�%d�Ż�������ȡ��%d�Ų�Ʒ\n", bufferNum_C + 1, Count_C);
			bufferNum_C = (bufferNum_C + 1) % 4;//��֤���������ѭ��
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
	//��ʼ���ź�������������СΪ4
	Empty = CreateSemaphore(NULL, 4, 4, NULL);
	Full = CreateSemaphore(NULL, 0, 4, NULL);
	Mutex = CreateSemaphore(NULL, 1, 1, NULL);
	//������1����������3��
	HANDLE hThread[4];
	hThread[0] = (HANDLE)_beginthreadex(NULL, 0, ProducerThread, NULL, 0, NULL);
	hThread[1] = (HANDLE)_beginthreadex(NULL, 0, ConsumerThread, NULL, 0, NULL);
	hThread[2] = (HANDLE)_beginthreadex(NULL, 0, ConsumerThread, NULL, 0, NULL);
	hThread[3] = (HANDLE)_beginthreadex(NULL, 0, ConsumerThread, NULL, 0, NULL);

	WaitForMultipleObjects(4, hThread, TRUE, INFINITE);//���̵߳ȴ����������ߺ�������������
	for (int i = 0; i < 4; i++)
		CloseHandle(hThread[i]);

	//�����ź���
	CloseHandle(Empty);
	CloseHandle(Full);
	return 0;
}