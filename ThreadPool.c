#include "ThreadPool.h"
#include<assert.h>
//��ʼ��
ThreadPool* initThreadPool(EventLoop* mainLoop, int count) {
	ThreadPool* threadPool = (ThreadPool*)malloc(sizeof(ThreadPool));
	threadPool->index = 0;//Ĭ���±��0��ʼ
	threadPool->isRun = false;
	threadPool->threadNum = count;
	threadPool->workThreads = (WorkThread*)malloc(sizeof(WorkThread)*count);
	threadPool->mainLoop = mainLoop;
	return threadPool;
}

//�����̳߳�
void threadPoolRun(ThreadPool* threadPool) {
	printf("threadPoolRun...\n");
	assert(threadPool && !threadPool->isRun);//�̳߳ر���ʼ����û�п�ʼ����
	if (threadPool->mainLoop->threadID != pthread_self()) {//�����߳���������
		exit(0);
	}
	threadPool->isRun = true;
	printf("run work thread...\n");
	if (threadPool->threadNum) {
		for (int i = 0; i < threadPool->threadNum; i++) {//�����̳߳��еĹ����߳�
			initWorkThread(&threadPool->workThreads[i], i);
			printf("Ready...workThreadRun....\n");
			workThreadRun(&threadPool->workThreads[i]);
		}
	}
}
//ȡ���̳߳��е�ĳһ���̵߳ķ�Ӧ��ʵ��
EventLoop* takeWorkerEventLoop(ThreadPool* threadPool) {
	assert(threadPool->isRun);
	if (threadPool->mainLoop->threadID != pthread_self()) {//�����߳�ִ��
		exit(0);
	}
	//�ҵ�һ�����̲߳����ط�Ӧ��ʵ�������û�����߳̾ͷ������̷߳�Ӧ��ʵ��
	EventLoop* eventLoop = threadPool->mainLoop;
	if (threadPool->threadNum > 0) {//�����߳�
		eventLoop = threadPool->workThreads[threadPool->index].eventloop;
		threadPool->index = (threadPool->index + 1) % threadPool->threadNum;
	}
	return eventLoop;
}