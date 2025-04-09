#include<stdio.h>
#include<sys/select.h>
#include"Dispathcer.h"
#include"EventLoop.h"
#define MAX 1024
struct selectData {
	fd_set readsets;
	fd_set writesets;
};

//init  ��ʼ��epoll,poll,select��Ҫ�����ݿ�
static void* selectInit();
//���
static int selectAdd(Channel* channel, EventLoop* eventloop);
//ɾ��
static int selectremove(Channel* channel, EventLoop* eventloop);
//�޸�
static int selectModify(Channel* channel, EventLoop* eventloop);
// �¼����
static int selectDispatch(EventLoop* eventloop, int timeout);//��λs
//�������
static int selectClear(EventLoop* eventloop);

Dispathcer selectDispatchcer = {
	selectInit,
	selectAdd,
	selectremove,
	selectModify,
	selectDispatch,
	selectClear
};

//init  ��ʼ��epoll,poll,select��Ҫ�����ݿ�
static void* selectInit() {
	struct selectData* t = (struct selectData*)malloc(sizeof(struct selectData));
	FD_ZERO(&t->readsets);
	FD_ZERO(&t->writesets);
	return t;
}
//���
static int selectAdd(Channel* channel, EventLoop* eventloop) {
	struct selectData* data = (struct selectData*)eventloop->data;
	if (channel->fd >= MAX) return -1;
	if (channel->events & ReadEvent) {//���¼�
		FD_SET(channel->fd, &data->readsets);
	}
	if (channel->events & WriteEvent) {//д�¼�
		FD_SET(channel->fd, &data->writesets);
	}
	return 0;
}
//ɾ��
static int selectremove(Channel* channel, EventLoop* eventloop) {
	struct selectData* data = (struct selectData*)eventloop->data;
	if (channel->fd >= MAX) return -1;
	if (channel->events & ReadEvent) {//ɾ�����¼�
		FD_CLR(channel->fd, &data->readsets);
	}
	if (channel->events & WriteEvent) {//д�¼�
		FD_CLR(channel->fd, &data->writesets);
	}
	channel->destoryCallback(channel->arg);
	return 0;
}
//�޸�
static int selectModify(Channel* channel, EventLoop* eventloop) {////��������������������������������
	struct selectData* data = (struct selectData*)eventloop->data;
	if (channel->fd >= MAX) return -1;
	//�ж��¼������
	if (channel->events & ReadEvent) {//���¼�
		FD_SET(channel->fd, &data->readsets);
	}
	else {//û�о�ɾ��
		FD_CLR(channel->fd, &data->readsets);
	}

	//��д�¼������
	if (channel->events & WriteEvent) {//д�¼�
		FD_SET(channel->fd, &data->writesets);
	}
	else {//û�о�ɾ��
		FD_CLR(channel->fd, &data->writesets);
	}
	return 0;
}
// �¼����
//timeout ��λs
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
		if (FD_ISSET(i, &readsets)) {//�������¼�
			eventActive(eventloop, i, ReadEvent);
		}
		if (FD_ISSET(i, &writesets)) {//����д�¼�
			eventActive(eventloop, i, WriteEvent);
		}
	}
	return 0;
}
//�������
static int selectClear(EventLoop* eventloop) {
	struct selectData* data = (struct selectData*)eventloop->data;
	free(data);
	return 0;
}
