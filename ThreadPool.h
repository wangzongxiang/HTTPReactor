#pragma once
#include"EventLoop.h"
#include"WorkThread.h"
//�̳߳�
typedef struct ThreadPool
{
	//���̷߳�Ӧ��ģ��
	EventLoop* mainLoop;
	//�ж��̳߳��Ƿ�����
	bool isRun;
	//�̳߳����̵߳�����
	int threadNum;
	//�̳߳��е��߳�
	WorkThread* workThreads;
	//index ȡ���̵߳ı��
	int index;
}ThreadPool;
//��ʼ��
ThreadPool* initThreadPool(EventLoop* mainLoop,int count);
//�����̳߳�
void threadPoolRun(ThreadPool* threadPool);
//ȡ���̳߳��е�ĳһ���߳�
EventLoop* takeWorkerEventLoop(ThreadPool* threadPool);


