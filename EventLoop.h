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
//type枚举类型
enum ElemType {
	ADD,//添加操作
	DEL,//删除操作
	MOD//修改操作
};
//定义任务队列的节点
typedef struct  ChannelElement {
	int type;
	Channel* channel;
	struct  ChannelElement* next;
}ChannelElement;
//反应堆
typedef struct EventLoop {
	bool isQuit;
	Dispathcer* dispatcher;
	void* data;//用到的数据
	//任务队列
	ChannelElement* head;//头结点
	ChannelElement* tail;//尾结点
	//chnannelMap
	ChannelMap* channelmap;
	//线程id,name
	pthread_t threadID;
	char threadName[32];
	//互斥锁
	pthread_mutex_t mutex;
	//文件描述符对，用于子线程处理主线程任务队列的任务时，主动唤醒子线程
	int socketPair[2];
}EventLoop;
//初始化
EventLoop* eventLoopInit();
EventLoop* EventLoopInitN(char* threadName);
//启动反应堆
int EventLoopRun(EventLoop* eventloop);
//处理被激活的文件描述符函数
int eventActive(EventLoop* eventloop, int fd, int event);
//向任务队列中添加任务
int eventLoopAddTask(Channel* channel, EventLoop* evenloop, int type);
//处理队列中的任务
int eventLoopProcessTask(EventLoop* evenloop);
//添加文件描述符到用于监测的dispathcer
int eventLoopAdd(Channel* channel, EventLoop* evenloop);
//删除文件描述符到用于监测的dispathcer
int eventLoopDel(Channel* channel, EventLoop* evenloop);
//修改文件描述符到用于监测的dispathcer
int eventLoopMod(Channel* channel, EventLoop* evenloop);
