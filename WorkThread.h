#pragma once
#include<pthread.h>
#include"EventLoop.h"
typedef struct EventLoop EventLoop;

typedef struct WorkThread {
	pthread_t threadID;
	char name[24];
	pthread_mutex_t mutex;//������
	pthread_cond_t cood;//��������
	EventLoop* eventloop;//��Ӧ��
}WorkThread;

//��ʼ��
int initWorkThread(WorkThread* thread,int index);
//�����߳�
int workThreadRun(WorkThread* thread);
