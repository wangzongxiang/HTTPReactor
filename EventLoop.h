#pragma once
#include "Dispathcer.h"
#include "ChannelMap.h"
#include <pthread.h>
#include "Channel.h"

typedef struct Channel Channel;
typedef struct Dispathcer Dispathcer;
typedef struct ChannelMap ChannelMap;
typedef struct Dispathcer Dispathcer;

extern Dispathcer epollDispatchcer;
extern Dispathcer pollDispatchcer;
extern Dispathcer selectDispatchcer;
//typeö������
enum ElemType {
	ADD,//��Ӳ���
	DEL,//ɾ������
	MOD//�޸Ĳ���
};
//����������еĽڵ�
typedef struct  ChannelElement {
	int type;
	Channel* channel;
	struct  ChannelElement* next;
}ChannelElement;
//��Ӧ��
typedef struct EventLoop {
	bool isQuit;
	Dispathcer* dispatcher;
	void* data;//�õ�������
	//�������
	ChannelElement* head;//ͷ���
	ChannelElement* tail;//β���
	//chnannelMap
	ChannelMap* channelmap;
	//�߳�id,name
	pthread_t threadID;
	char threadName[32];
	//������
	pthread_mutex_t mutex;
	//�ļ��������ԣ��������̴߳������߳�������е�����ʱ�������������߳�
	int socketPair[2];
}EventLoop;
//��ʼ��
EventLoop* eventLoopInit();
EventLoop* EventLoopInitN(char* threadName);
//������Ӧ��
int EventLoopRun(EventLoop* eventloop);
//����������ļ�����������
int eventActive(EventLoop* eventloop, int fd, int event);
//������������������
int eventLoopAddTask(Channel* channel, EventLoop* evenloop, int type);
//��������е�����
int eventLoopProcessTask(EventLoop* evenloop);
//����ļ������������ڼ���dispathcer
int eventLoopAdd(Channel* channel, EventLoop* evenloop);
//ɾ���ļ������������ڼ���dispathcer
int eventLoopDel(Channel* channel, EventLoop* evenloop);
//�޸��ļ������������ڼ���dispathcer
int eventLoopMod(Channel* channel, EventLoop* evenloop);
