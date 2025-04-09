#pragma once
#include"Buffer.h"
#include<stdio.h>
//����״̬��ö��
enum HttpStateCode {
	Unknown,
	OK=200,
	MovePermanetly=301,
	MoveTemporarily=302,
	BadRequest=400,
	NotFound =404
};
//������Ӧͷ�ṹ��
typedef struct ResponseHeader {
	char key[32];
	char value[128];
}ResponseHeader;
//���庯��ָ��
typedef void (*responseBody)(char* fileName,Buffer* sendBuffer,int socket);
typedef struct HttpResponse
{
	//״̬�У�״̬�� ״̬������ ��Ӧ�汾
	enum HttpStateCode stateCode;
	char statusMsg[128];//״̬����
	//��Ӧͷ  ��ֵ��
	ResponseHeader* responseHeader;
	int headerNum;//�洢�ĸ���
	responseBody sendFunc;//����ĺ���ָ��
	char* filename;
}HttpResponse;
//��ʼ��
HttpResponse* HttpResponseInit();
//����
void httpResponseDestroy(HttpResponse* httpResponse);
//�����Ӧͷ
void httpResponseAddHeader(HttpResponse* httpResponse, char* key, char* value);
//��֯��Ӧ����
void httpResponsePrepareMsg(HttpResponse* httpResponse, Buffer* sendBuffer, int socket);
