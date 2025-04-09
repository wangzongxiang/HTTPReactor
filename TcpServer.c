#include "TcpServer.h"
#include<arpa/inet.h>
#include"TcpConnection.h"
#include <sys/socket.h>
#include<fcntl.h>
//初始化
//size 线程池中工作线程的个数
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
	//创建用于监听的文字描述符
	int lfd = socket(AF_INET, SOCK_STREAM, 0);
	if (lfd == -1) {
		perror("scoket");
		return NULL;
	}
	//设置端口复用
	int opt = 1;
	int ret = setsockopt(lfd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
	if (ret == -1) {
		perror("setsockopt");
		return NULL;
	}
	//绑定
	struct sockaddr_in addr;
	addr.sin_family = AF_INET;
	addr.sin_port = htons(port);
	addr.sin_addr.s_addr = INADDR_ANY;
	ret = bind(lfd, (struct sockaddr*)&addr, sizeof(addr));
	if (ret == -1) {
		perror("bind ");
		return NULL;
	}
	//设置监听
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
	//从线程池中取出子线程的反应堆
	EventLoop* sonThread = takeWorkerEventLoop(tcpServer->threadPool);
	//将cfd放入TcpConnection中
	initTcpConnection(ret, sonThread);
	return 0;
}
void tcpServerRun(TcpServer* tcpServer) {
	printf("tcpServerRun...\n");
	threadPoolRun(tcpServer->threadPool);//启动线程池
	Channel* channel = channelInit(tcpServer->listener->fd, ReadEvent, acceptConnection, NULL, NULL, tcpServer);
	eventLoopAddTask(channel, tcpServer->mainEventLoop, ADD);//将用于监听的文件描述符添加到任务队列
	EventLoopRun(tcpServer->mainEventLoop);//启动主线程反应堆模型
}
