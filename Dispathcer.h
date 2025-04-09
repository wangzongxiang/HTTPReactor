#pragma once
#include"Channel.h"
#include"EventLoop.h"
//用于存储io多路复用的函数指针

typedef struct EventLoop EventLoop;
typedef struct Channel Channel;
typedef struct Channel Channel;

typedef struct Dispathcer {
	//init  初始化epoll,poll,select需要的数据块
	void* (*init)();
	//添加
	int (*add)(Channel* channel,EventLoop* eventloop);
	//删除
	int (*remove)(Channel* channel, EventLoop* eventloop);
	//修改
	int (*modify)(Channel* channel, EventLoop* eventloop);
	// 事件监测
	int (*dispatch)(EventLoop* eventloop,int timeout);//单位s
	//清除数据
	int (*clear)(EventLoop* eventloop);
}Dispathcer;
