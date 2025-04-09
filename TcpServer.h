#pragma once
#include"EventLoop.h"
#include"ThreadPool.h"
typedef struct Listener {
	int fd;//���ڼ������ļ�������
	unsigned short port;//�˿�
}Listener;

typedef struct TcpServer {
	EventLoop* mainEventLoop;//���̷߳�Ӧ��
	ThreadPool* threadPool;
	int threadNum;//�̳߳������̵߳ĸ���
	Listener* listener;
}TcpServer;

//��ʼ��
TcpServer* initTcpServer(unsigned short port, int size);
//��ʼ������
Listener* initListener(unsigned short port);
//����
void tcpServerRun(TcpServer* tcpServer);
