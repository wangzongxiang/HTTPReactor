#pragma once
#include"Channel.h"
#include"Buffer.h"
#include"EventLoop.h"
#include"HttpRequest.h"
#include"HttpResponse.h"
typedef struct TcpConnection {//一个连接对应一个tcpconnection
	EventLoop* evLoop;
	Channel* channel;
	Buffer* readBuffer;
	Buffer* writeBuffer;
	char name[32];
	HttpRequest* httpRequest;//http请求
	HttpResponse* httpResponse;//http响应
}TcpConnection;

int ProcessRead(void* arg);
int ProcessWrite(void* arg);
//初始化
TcpConnection* initTcpConnection(int fd, EventLoop* evLoop);
//资源释放
int tcpConnectionDestory(void* arg);
