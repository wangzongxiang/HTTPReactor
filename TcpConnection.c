#include "TcpConnection.h"
#include"EventLoop.h"
int ProcessRead(void* arg) {
	TcpConnection* tcpconnection = (TcpConnection*)arg;
	//接受数据
	int count = bufferSocketRead(tcpconnection->readBuffer, tcpconnection->channel->fd);
	if (count > 0) {//接收到HTTP请求 解析
		//解析并处理请求
		//添加后的弊端就是要全部写完才可以发送，可能内存不够，采用边读边发送的方式
		//writeEventEnable(tcpconnection->channel, true);
		//eventLoopAddTask(tcpconnection->channel, tcpconnection->evLoop, MOD);//添加写事件
		bool flag = praseHttpRequest(tcpconnection->httpRequest, tcpconnection->readBuffer, tcpconnection->httpResponse, tcpconnection->writeBuffer, tcpconnection->channel->fd);
		if (!flag) {//解析失败 //返回简单的HTML
			char* errMsg = "Http/1.1 400 Bad Request\r\n\r\n";
			bufferAppendString(tcpconnection->writeBuffer, errMsg);
		}
	}
	//断开连接
	eventLoopAddTask(tcpconnection->channel, tcpconnection->evLoop, DEL);
	return 0;
}

int ProcessWrite(void* arg) {//将writeBuffer中的数据写至写缓冲区
	printf("write http response\n");
	TcpConnection* tcpconnection = (TcpConnection*)arg;
	//发送数据给客户端
	int flag=bufferSendData(tcpconnection->writeBuffer, tcpconnection->channel->fd);
	if (flag > 0) {//判断是否全部发出去
		if (tcpconnection->writeBuffer->writePos - tcpconnection->readBuffer->readPos == 0) {//全部发送出去
			////不在检测该结点的写事件!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
			//writeEventEnable(tcpconnection->channel, false);
			//写完就断开连接
			eventLoopAddTask(tcpconnection->channel, tcpconnection->evLoop, DEL);
		}
	}
	return 0;
}


	
//初始化
TcpConnection* initTcpConnection(int fd, EventLoop* evLoop) {
	TcpConnection* tcpconnection = (TcpConnection*)malloc(sizeof(TcpConnection));
	tcpconnection->evLoop = evLoop;
	tcpconnection->readBuffer = initBuffer(10240);
	tcpconnection->writeBuffer = initBuffer(10240);
	tcpconnection->httpRequest = HttpRequestInit();
	tcpconnection->httpResponse= HttpResponseInit();
	sprintf(tcpconnection->name, "Connection-%d", fd);
	tcpconnection->channel = channelInit(fd, ReadEvent, ProcessRead, ProcessWrite, tcpConnectionDestory,tcpconnection);
	printf("add channel in sonthread....\n");
	eventLoopAddTask(tcpconnection->channel, evLoop, ADD);//添加到反应堆
	return tcpconnection;
}

//资源释放
int tcpConnectionDestory(void* arg) {
	TcpConnection* tcpConnection = (TcpConnection*)arg;
	if (tcpConnection == NULL) return -1;
	if (tcpConnection->readBuffer && tcpConnection->readBuffer->writePos - tcpConnection->readBuffer->readPos == 0
		&& tcpConnection->writeBuffer->writePos - tcpConnection->writeBuffer->readPos == 0) {
		bufferDestory(tcpConnection->readBuffer);
		bufferDestory(tcpConnection->writeBuffer);
		httpRequestDestory(tcpConnection->httpRequest);
		httpResponseDestroy(tcpConnection->httpResponse);
		channelDestory(tcpConnection->channel,tcpConnection->evLoop);
		free(tcpConnection);
	}
	return 0;
}
