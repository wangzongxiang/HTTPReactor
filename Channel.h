#pragma once//��ֹ�ظ�����
#include<stdbool.h>
#include<stdlib.h>
#include<stdio.h>
#include"EventLoop.h"
//���庯��ָ��
typedef int (*handleFunc)(void* arg);


enum FDevent {
ReadEvent = 0x02,//�����Ƶڶ�λ��0��1��ʾ�Ƿ��
WriteEvent = 0x04//�����Ƶ���λ��0��1��ʾ�Ƿ�д
};

typedef struct Channel
{

	//�ļ�������
	int fd;
	//�¼�
	int events;
	//�ص�����
	handleFunc readCallback;//��
	handleFunc writeCallback;//д
	handleFunc destoryCallback;//����channelɾ����Ӧ��tcpconnection
	//�ص������Ĳ���
	void* arg;
}Channel;
typedef struct EventLoop EventLoop;
//��ʼ��
Channel* channelInit(int fd,int events, handleFunc readCallback, handleFunc writeCallback, handleFunc destoryCallback,void* arg);
void  channelDestory(Channel* channel, EventLoop* eventLoop);
//�޸�fd��д�¼�
void writeEventEnable(Channel* channel,bool flag);
// �ж��Ƿ���Ҫ����ļ���������д�¼�
bool isWriteEventEnable(Channel* channel);