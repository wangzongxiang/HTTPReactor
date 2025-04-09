#include "TcpConnection.h"
#include"EventLoop.h"
int ProcessRead(void* arg) {
	TcpConnection* tcpconnection = (TcpConnection*)arg;
	//��������
	int count = bufferSocketRead(tcpconnection->readBuffer, tcpconnection->channel->fd);
	if (count > 0) {//���յ�HTTP���� ����
		//��������������
		//��Ӻ�ı׶˾���Ҫȫ��д��ſ��Է��ͣ������ڴ治�������ñ߶��߷��͵ķ�ʽ
		//writeEventEnable(tcpconnection->channel, true);
		//eventLoopAddTask(tcpconnection->channel, tcpconnection->evLoop, MOD);//���д�¼�
		bool flag = praseHttpRequest(tcpconnection->httpRequest, tcpconnection->readBuffer, tcpconnection->httpResponse, tcpconnection->writeBuffer, tcpconnection->channel->fd);
		if (!flag) {//����ʧ�� //���ؼ򵥵�HTML
			char* errMsg = "Http/1.1 400 Bad Request\r\n\r\n";
			bufferAppendString(tcpconnection->writeBuffer, errMsg);
		}
	}
	//�Ͽ�����
	eventLoopAddTask(tcpconnection->channel, tcpconnection->evLoop, DEL);
	return 0;
}

int ProcessWrite(void* arg) {//��writeBuffer�е�����д��д������
	printf("write http response\n");
	TcpConnection* tcpconnection = (TcpConnection*)arg;
	//�������ݸ��ͻ���
	int flag=bufferSendData(tcpconnection->writeBuffer, tcpconnection->channel->fd);
	if (flag > 0) {//�ж��Ƿ�ȫ������ȥ
		if (tcpconnection->writeBuffer->writePos - tcpconnection->readBuffer->readPos == 0) {//ȫ�����ͳ�ȥ
			////���ڼ��ý���д�¼�!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
			//writeEventEnable(tcpconnection->channel, false);
			//д��ͶϿ�����
			eventLoopAddTask(tcpconnection->channel, tcpconnection->evLoop, DEL);
		}
	}
	return 0;
}


	
//��ʼ��
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
	eventLoopAddTask(tcpconnection->channel, evLoop, ADD);//��ӵ���Ӧ��
	return tcpconnection;
}

//��Դ�ͷ�
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
