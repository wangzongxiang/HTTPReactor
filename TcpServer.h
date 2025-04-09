#pragma once
#include"EventLoop.h"
#include"ThreadPool.h"
typedef struct Listener {
	int fd;//用于监听的文件描述符
	unsigned short port;//端口
}Listener;

typedef struct TcpServer {
	EventLoop* mainEventLoop;//主线程反应堆
	ThreadPool* threadPool;
	int threadNum;//线程池中子线程的个数
	Listener* listener;
}TcpServer;

//初始化
TcpServer* initTcpServer(unsigned short port, int size);
//初始化监听
Listener* initListener(unsigned short port);
//启动
void tcpServerRun(TcpServer* tcpServer);
