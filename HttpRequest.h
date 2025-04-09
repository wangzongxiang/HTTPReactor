#pragma once
#include<string.h>
#include"Buffer.h"
#include<stdbool.h>
#include"HttpResponse.h"
//请求头键值对
typedef struct RequestHeader {
	char* key;
	char* value;
}RequestHeader;
//当前解析状态
enum HttpRequestState {
	parseReqLine,//请求行
	parseReqHeadLine,//请求头
	parseReqBodyLine,//请求数据块
	parseReqDone,//请求完成
}HttpRequestState;
//http请求
typedef struct HttpRequest
{
	char* method;
	char* url;
	char* version;
	RequestHeader* requestHeader;//存储键值对
	int size;//requestHeader内存储的个数
	enum HttpRequestState state;
}HttpRequest;

//初始化
HttpRequest* HttpRequestInit();
//重置
//当多次请求时，需要销毁上次创建的堆内存，因此需要进行重置
void httpRequestReset(HttpRequest* httpRequest);
void httpRequestResetEx(HttpRequest* httpRequest);
void httpRequestDestory(HttpRequest* httpRequest);
enum HttpRequestState getHttpRequestState(HttpRequest* httpRequest);
//添加请求头
void httpRequestAddHeader(HttpRequest* httpRequest,char* key,char* value);
//根据KEY获取value
char* getHttpRequestHeader(HttpRequest* httpRequest, char* key);
//解析请求行
bool praseHttpReuqestLine(HttpRequest* httpRequest,Buffer* buffer);
//解析请求头
bool praseHttpRequestHeadLine(HttpRequest* httpRequest, Buffer* buffer);
//解析HTTP请求
bool praseHttpRequest(HttpRequest* httpRequest, Buffer* buffer, HttpResponse* httpResponse, Buffer* sendBuffer, int socket);
//处理HTTP请求
bool processHttpRequest(HttpRequest* httpRequest, HttpResponse* httpResponse);
//判断请求文件类型
const char* getFileType(const char* name);
//发送文件
int sendFile(const char* filename, Buffer* sendBuffer, int cfd);
//发送目录
int sendDir(const char* dirName, Buffer* sendBuffer, int cfd);


