#pragma once
#include"Channel.h"
#include"EventLoop.h"
//���ڴ洢io��·���õĺ���ָ��

typedef struct EventLoop EventLoop;
typedef struct Channel Channel;
typedef struct Channel Channel;

typedef struct Dispathcer {
	//init  ��ʼ��epoll,poll,select��Ҫ�����ݿ�
	void* (*init)();
	//���
	int (*add)(Channel* channel,EventLoop* eventloop);
	//ɾ��
	int (*remove)(Channel* channel, EventLoop* eventloop);
	//�޸�
	int (*modify)(Channel* channel, EventLoop* eventloop);
	// �¼����
	int (*dispatch)(EventLoop* eventloop,int timeout);//��λs
	//�������
	int (*clear)(EventLoop* eventloop);
}Dispathcer;
