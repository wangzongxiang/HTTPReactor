#include "HttpResponse.h"
#include<string.h>
#include<stdio.h>
#include<stdlib.h>
#define MAX 16
//初始化
HttpResponse* HttpResponseInit() {
	HttpResponse* httpResponse = (HttpResponse*)malloc(sizeof(HttpResponse));
	httpResponse->headerNum = 0;
	httpResponse->responseHeader=(ResponseHeader*)malloc(sizeof(ResponseHeader*)* MAX);
	httpResponse->sendFunc = NULL;//函数指针为空
	httpResponse->stateCode = Unknown;
	//初始化数组
	bzero(httpResponse->responseHeader, sizeof(ResponseHeader*) * MAX);
	bzero(httpResponse->statusMsg, sizeof(httpResponse->statusMsg));
	return httpResponse;
}

//销毁
void httpResponseDestroy(HttpResponse* httpResponse) {
	if (httpResponse == NULL) return;
	free(httpResponse->responseHeader);
	free(httpResponse);
}
//添加响应头
void httpResponseAddHeader(HttpResponse* httpResponse, char* key, char* value) {
	if (httpResponse == NULL || key == NULL || value == NULL) return;
	memcpy(httpResponse->responseHeader[httpResponse->headerNum].key, key, strlen(key));
	memcpy(httpResponse->responseHeader[httpResponse->headerNum].value, key, strlen(value));
	httpResponse->headerNum++;
}

//组织响应数据
void httpResponsePrepareMsg(HttpResponse* httpResponse, Buffer* sendBuffer, int socket) {
	//状态行
	char temp[1024] = {0};
	sprintf(temp,"HTTP/1.1 %d %s\r\n", httpResponse->stateCode, httpResponse->statusMsg);
	bufferAppendString(sendBuffer,temp);
	//响应头
	for (int i = 0; i < httpResponse->headerNum; i++) {
		char t[128] = {0};
		sprintf(t, "%s: %s\r\n", httpResponse->responseHeader[i].key, httpResponse->responseHeader[i].value);
		bufferAppendString(sendBuffer, t);
	}
	//空行
	bufferAppendString(sendBuffer, "\r\n");
	bufferSendData(sendBuffer, socket);//先发送一部分
	//响应数据  (*responseBody)(char* fileName,Buffer* sendBuffer,int socket);
	httpResponse->sendFunc(httpResponse->filename, sendBuffer, socket);//调用函数指针指向的函数
}