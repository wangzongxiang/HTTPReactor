#pragma once
#include"EventLoop.h"
#include"WorkThread.h"
//线程池
typedef struct ThreadPool
{
	//主线程反应堆模型
	EventLoop* mainLoop;
	//判断线程池是否启动
	bool isRun;
	//线程池中线程的数量
	int threadNum;
	//线程池中的线程
	WorkThread* workThreads;
	//index 取出线程的编号
	int index;
}ThreadPool;
//初始化
ThreadPool* initThreadPool(EventLoop* mainLoop,int count);
//运行线程池
void threadPoolRun(ThreadPool* threadPool);
//取出线程池中的某一个线程
EventLoop* takeWorkerEventLoop(ThreadPool* threadPool);


