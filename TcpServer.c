#include "TcpServer.h"
#include<arpa/inet.h>
#include"TcpConnection.h"
#include <sys/socket.h>
#include<fcntl.h>
//��ʼ��
//size �̳߳��й����̵߳ĸ���
TcpServer* initTcpServer(unsigned short port, int size) {//
	printf("initTcpServer...\n");
	TcpServer* tcpServer = (TcpServer*)malloc(sizeof(TcpServer));
	tcpServer->mainEventLoop = eventLoopInit();
	tcpServer->threadNum = size;
	tcpServer->threadPool = initThreadPool(tcpServer->mainEventLoop, tcpServer->threadNum);
	tcpServer->listener = initListener(port);
	return tcpServer;
}

Listener* initListener(unsigned short port) {
	printf("initListener...\n");
	Listener* listener = (Listener*)malloc(sizeof(Listener));
	//�������ڼ���������������
	int lfd = socket(AF_INET, SOCK_STREAM, 0);
	if (lfd == -1) {
		perror("scoket");
		return NULL;
	}
	//���ö˿ڸ���
	int opt = 1;
	int ret = setsockopt(lfd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
	if (ret == -1) {
		perror("setsockopt");
		return NULL;
	}
	//��
	struct sockaddr_in addr;
	addr.sin_family = AF_INET;
	addr.sin_port = htons(port);
	addr.sin_addr.s_addr = INADDR_ANY;
	ret = bind(lfd, (struct sockaddr*)&addr, sizeof(addr));
	if (ret == -1) {
		perror("bind ");
		return NULL;
	}
	//���ü���
	ret = listen(lfd, 128);
	listener->fd = lfd;
	listener->port = port;
	return listener;
}

int acceptConnection(void* arg) {
	printf("listen the acceptConnection...\n");
	TcpServer* tcpServer = (TcpServer*)arg;
	int ret = accept(tcpServer->listener->fd, NULL, NULL);
	if (ret == -1) return -1;
	//���̳߳���ȡ�����̵߳ķ�Ӧ��
	EventLoop* sonThread = takeWorkerEventLoop(tcpServer->threadPool);
	//��cfd����TcpConnection��
	initTcpConnection(ret, sonThread);
	return 0;
}
void tcpServerRun(TcpServer* tcpServer) {
	printf("tcpServerRun...\n");
	threadPoolRun(tcpServer->threadPool);//�����̳߳�
	Channel* channel = channelInit(tcpServer->listener->fd, ReadEvent, acceptConnection, NULL, NULL, tcpServer);
	eventLoopAddTask(channel, tcpServer->mainEventLoop, ADD);//�����ڼ������ļ���������ӵ��������
	EventLoopRun(tcpServer->mainEventLoop);//�������̷߳�Ӧ��ģ��
}
