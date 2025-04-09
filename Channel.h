#pragma once//防止重复包含
#include<stdbool.h>
#include<stdlib.h>
#include<stdio.h>
#include"EventLoop.h"
//定义函数指针
typedef int (*handleFunc)(void* arg);


enum FDevent {
ReadEvent = 0x02,//二进制第二位的0，1表示是否读
WriteEvent = 0x04//二进制第三位的0，1表示是否写
};

typedef struct Channel
{

	//文件描述符
	int fd;
	//事件
	int events;
	//回调函数
	handleFunc readCallback;//读
	handleFunc writeCallback;//写
	handleFunc destoryCallback;//根据channel删除对应的tcpconnection
	//回调函数的参数
	void* arg;
}Channel;
typedef struct EventLoop EventLoop;
//初始化
Channel* channelInit(int fd,int events, handleFunc readCallback, handleFunc writeCallback, handleFunc destoryCallback,void* arg);
void  channelDestory(Channel* channel, EventLoop* eventLoop);
//修改fd的写事件
void writeEventEnable(Channel* channel,bool flag);
// 判断是否需要检测文件描述符的写事件
bool isWriteEventEnable(Channel* channel);