#pragma once
#include"Buffer.h"
#include<stdio.h>
//定义状态码枚举
enum HttpStateCode {
	Unknown,
	OK=200,
	MovePermanetly=301,
	MoveTemporarily=302,
	BadRequest=400,
	NotFound =404
};
//定义响应头结构体
typedef struct ResponseHeader {
	char key[32];
	char value[128];
}ResponseHeader;
//定义函数指针
typedef void (*responseBody)(char* fileName,Buffer* sendBuffer,int socket);
typedef struct HttpResponse
{
	//状态行：状态码 状态码描述 响应版本
	enum HttpStateCode stateCode;
	char statusMsg[128];//状态描述
	//响应头  键值对
	ResponseHeader* responseHeader;
	int headerNum;//存储的个数
	responseBody sendFunc;//处理的函数指针
	char* filename;
}HttpResponse;
//初始化
HttpResponse* HttpResponseInit();
//销毁
void httpResponseDestroy(HttpResponse* httpResponse);
//添加响应头
void httpResponseAddHeader(HttpResponse* httpResponse, char* key, char* value);
//组织响应数据
void httpResponsePrepareMsg(HttpResponse* httpResponse, Buffer* sendBuffer, int socket);
