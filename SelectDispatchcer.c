#include<stdio.h>
#include<sys/select.h>
#include"Dispathcer.h"
#include"EventLoop.h"
#define MAX 1024
struct selectData {
	fd_set readsets;
	fd_set writesets;
};

//init  初始化epoll,poll,select需要的数据块
static void* selectInit();
//添加
static int selectAdd(Channel* channel, EventLoop* eventloop);
//删除
static int selectremove(Channel* channel, EventLoop* eventloop);
//修改
static int selectModify(Channel* channel, EventLoop* eventloop);
// 事件监测
static int selectDispatch(EventLoop* eventloop, int timeout);//单位s
//清除数据
static int selectClear(EventLoop* eventloop);

Dispathcer selectDispatchcer = {
	selectInit,
	selectAdd,
	selectremove,
	selectModify,
	selectDispatch,
	selectClear
};

//init  初始化epoll,poll,select需要的数据块
static void* selectInit() {
	struct selectData* t = (struct selectData*)malloc(sizeof(struct selectData));
	FD_ZERO(&t->readsets);
	FD_ZERO(&t->writesets);
	return t;
}
//添加
static int selectAdd(Channel* channel, EventLoop* eventloop) {
	struct selectData* data = (struct selectData*)eventloop->data;
	if (channel->fd >= MAX) return -1;
	if (channel->events & ReadEvent) {//读事件
		FD_SET(channel->fd, &data->readsets);
	}
	if (channel->events & WriteEvent) {//写事件
		FD_SET(channel->fd, &data->writesets);
	}
	return 0;
}
//删除
static int selectremove(Channel* channel, EventLoop* eventloop) {
	struct selectData* data = (struct selectData*)eventloop->data;
	if (channel->fd >= MAX) return -1;
	if (channel->events & ReadEvent) {//删除读事件
		FD_CLR(channel->fd, &data->readsets);
	}
	if (channel->events & WriteEvent) {//写事件
		FD_CLR(channel->fd, &data->writesets);
	}
	channel->destoryCallback(channel->arg);
	return 0;
}
//修改
static int selectModify(Channel* channel, EventLoop* eventloop) {////！！！！！！！！！！！！！！！！
	struct selectData* data = (struct selectData*)eventloop->data;
	if (channel->fd >= MAX) return -1;
	//有读事件就添加
	if (channel->events & ReadEvent) {//读事件
		FD_SET(channel->fd, &data->readsets);
	}
	else {//没有就删除
		FD_CLR(channel->fd, &data->readsets);
	}

	//有写事件就添加
	if (channel->events & WriteEvent) {//写事件
		FD_SET(channel->fd, &data->writesets);
	}
	else {//没有就删除
		FD_CLR(channel->fd, &data->writesets);
	}
	return 0;
}
// 事件监测
//timeout 单位s
static int selectDispatch(EventLoop* eventloop, int timeout) {
	struct selectData* data = (struct selectData*)eventloop->data;
	struct timeval val;
	val.tv_sec = timeout;
	val.tv_usec= 0;
	fd_set readsets = data->readsets;
	fd_set writesets=data->writesets;
	int ret = select(MAX, &readsets, &writesets,NULL, &val);
	if (ret == -1) {
		perror("select");
		exit(0);
	}

	for (int i = 0; i <MAX; i++) {
		if (FD_ISSET(i, &readsets)) {//触发读事件
			eventActive(eventloop, i, ReadEvent);
		}
		if (FD_ISSET(i, &writesets)) {//触发写事件
			eventActive(eventloop, i, WriteEvent);
		}
	}
	return 0;
}
//清除数据
static int selectClear(EventLoop* eventloop) {
	struct selectData* data = (struct selectData*)eventloop->data;
	free(data);
	return 0;
}
