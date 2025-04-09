#pragma once
#include"Channel.h"
#include"Buffer.h"
#include"EventLoop.h"
#include"HttpRequest.h"
#include"HttpResponse.h"
typedef struct TcpConnection {//һ�����Ӷ�Ӧһ��tcpconnection
	EventLoop* evLoop;
	Channel* channel;
	Buffer* readBuffer;
	Buffer* writeBuffer;
	char name[32];
	HttpRequest* httpRequest;//http����
	HttpResponse* httpResponse;//http��Ӧ
}TcpConnection;

int ProcessRead(void* arg);
int ProcessWrite(void* arg);
//��ʼ��
TcpConnection* initTcpConnection(int fd, EventLoop* evLoop);
//��Դ�ͷ�
int tcpConnectionDestory(void* arg);
