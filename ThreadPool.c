#include "ThreadPool.h"
#include<assert.h>
//初始化
ThreadPool* initThreadPool(EventLoop* mainLoop, int count) {
	ThreadPool* threadPool = (ThreadPool*)malloc(sizeof(ThreadPool));
	threadPool->index = 0;//默认下标从0开始
	threadPool->isRun = false;
	threadPool->threadNum = count;
	threadPool->workThreads = (WorkThread*)malloc(sizeof(WorkThread)*count);
	threadPool->mainLoop = mainLoop;
	return threadPool;
}

//运行线程池
void threadPoolRun(ThreadPool* threadPool) {
	printf("threadPoolRun...\n");
	assert(threadPool && !threadPool->isRun);//线程池被初始化且没有开始运行
	if (threadPool->mainLoop->threadID != pthread_self()) {//由主线程启动运行
		exit(0);
	}
	threadPool->isRun = true;
	printf("run work thread...\n");
	if (threadPool->threadNum) {
		for (int i = 0; i < threadPool->threadNum; i++) {//启动线程池中的工作线程
			initWorkThread(&threadPool->workThreads[i], i);
			printf("Ready...workThreadRun....\n");
			workThreadRun(&threadPool->workThreads[i]);
		}
	}
}
//取出线程池中的某一个线程的反应堆实例
EventLoop* takeWorkerEventLoop(ThreadPool* threadPool) {
	assert(threadPool->isRun);
	if (threadPool->mainLoop->threadID != pthread_self()) {//由主线程执行
		exit(0);
	}
	//找到一个子线程并返回反应堆实例，如果没有子线程就返回主线程反应堆实例
	EventLoop* eventLoop = threadPool->mainLoop;
	if (threadPool->threadNum > 0) {//有子线程
		eventLoop = threadPool->workThreads[threadPool->index].eventloop;
		threadPool->index = (threadPool->index + 1) % threadPool->threadNum;
	}
	return eventLoop;
}