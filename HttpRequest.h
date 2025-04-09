#pragma once
#include<string.h>
#include"Buffer.h"
#include<stdbool.h>
#include"HttpResponse.h"
//����ͷ��ֵ��
typedef struct RequestHeader {
	char* key;
	char* value;
}RequestHeader;
//��ǰ����״̬
enum HttpRequestState {
	parseReqLine,//������
	parseReqHeadLine,//����ͷ
	parseReqBodyLine,//�������ݿ�
	parseReqDone,//�������
}HttpRequestState;
//http����
typedef struct HttpRequest
{
	char* method;
	char* url;
	char* version;
	RequestHeader* requestHeader;//�洢��ֵ��
	int size;//requestHeader�ڴ洢�ĸ���
	enum HttpRequestState state;
}HttpRequest;

//��ʼ��
HttpRequest* HttpRequestInit();
//����
//���������ʱ����Ҫ�����ϴδ����Ķ��ڴ棬�����Ҫ��������
void httpRequestReset(HttpRequest* httpRequest);
void httpRequestResetEx(HttpRequest* httpRequest);
void httpRequestDestory(HttpRequest* httpRequest);
enum HttpRequestState getHttpRequestState(HttpRequest* httpRequest);
//�������ͷ
void httpRequestAddHeader(HttpRequest* httpRequest,char* key,char* value);
//����KEY��ȡvalue
char* getHttpRequestHeader(HttpRequest* httpRequest, char* key);
//����������
bool praseHttpReuqestLine(HttpRequest* httpRequest,Buffer* buffer);
//��������ͷ
bool praseHttpRequestHeadLine(HttpRequest* httpRequest, Buffer* buffer);
//����HTTP����
bool praseHttpRequest(HttpRequest* httpRequest, Buffer* buffer, HttpResponse* httpResponse, Buffer* sendBuffer, int socket);
//����HTTP����
bool processHttpRequest(HttpRequest* httpRequest, HttpResponse* httpResponse);
//�ж������ļ�����
const char* getFileType(const char* name);
//�����ļ�
int sendFile(const char* filename, Buffer* sendBuffer, int cfd);
//����Ŀ¼
int sendDir(const char* dirName, Buffer* sendBuffer, int cfd);


