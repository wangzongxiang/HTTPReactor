#pragma once
#include<pthread.h>
#include"EventLoop.h"
typedef struct EventLoop EventLoop;

typedef struct WorkThread {
	pthread_t threadID;
	char name[24];
	pthread_mutex_t mutex;//互斥锁
	pthread_cond_t cood;//条件变量
	EventLoop* eventloop;//反应堆
}WorkThread;

//初始化
int initWorkThread(WorkThread* thread,int index);
//启动线程
int workThreadRun(WorkThread* thread);
