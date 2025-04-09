#include "HttpResponse.h"
#include<string.h>
#include<stdio.h>
#include<stdlib.h>
#define MAX 16
//��ʼ��
HttpResponse* HttpResponseInit() {
	HttpResponse* httpResponse = (HttpResponse*)malloc(sizeof(HttpResponse));
	httpResponse->headerNum = 0;
	httpResponse->responseHeader=(ResponseHeader*)malloc(sizeof(ResponseHeader*)* MAX);
	httpResponse->sendFunc = NULL;//����ָ��Ϊ��
	httpResponse->stateCode = Unknown;
	//��ʼ������
	bzero(httpResponse->responseHeader, sizeof(ResponseHeader*) * MAX);
	bzero(httpResponse->statusMsg, sizeof(httpResponse->statusMsg));
	return httpResponse;
}

//����
void httpResponseDestroy(HttpResponse* httpResponse) {
	if (httpResponse == NULL) return;
	free(httpResponse->responseHeader);
	free(httpResponse);
}
//�����Ӧͷ
void httpResponseAddHeader(HttpResponse* httpResponse, char* key, char* value) {
	if (httpResponse == NULL || key == NULL || value == NULL) return;
	memcpy(httpResponse->responseHeader[httpResponse->headerNum].key, key, strlen(key));
	memcpy(httpResponse->responseHeader[httpResponse->headerNum].value, key, strlen(value));
	httpResponse->headerNum++;
}

//��֯��Ӧ����
void httpResponsePrepareMsg(HttpResponse* httpResponse, Buffer* sendBuffer, int socket) {
	//״̬��
	char temp[1024] = {0};
	sprintf(temp,"HTTP/1.1 %d %s\r\n", httpResponse->stateCode, httpResponse->statusMsg);
	bufferAppendString(sendBuffer,temp);
	//��Ӧͷ
	for (int i = 0; i < httpResponse->headerNum; i++) {
		char t[128] = {0};
		sprintf(t, "%s: %s\r\n", httpResponse->responseHeader[i].key, httpResponse->responseHeader[i].value);
		bufferAppendString(sendBuffer, t);
	}
	//����
	bufferAppendString(sendBuffer, "\r\n");
	bufferSendData(sendBuffer, socket);//�ȷ���һ����
	//��Ӧ����  (*responseBody)(char* fileName,Buffer* sendBuffer,int socket);
	httpResponse->sendFunc(httpResponse->filename, sendBuffer, socket);//���ú���ָ��ָ��ĺ���
}