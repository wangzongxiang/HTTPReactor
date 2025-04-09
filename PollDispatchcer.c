#include <poll.h>
#include"Dispathcer.h"
#define MAX 1024
struct pollData {
	int maxfd;
	struct pollfd fds[MAX];
};
//init  ��ʼ��epoll,poll,select��Ҫ�����ݿ�
static void* pollInit();
//���
static int pollAdd(Channel* channel, EventLoop* eventloop);
//ɾ��
static int pollremove(Channel* channel, EventLoop* eventloop);
//�޸�
static int pollModify(Channel* channel, EventLoop* eventloop);
// �¼����
static int pollDispatch(EventLoop* eventloop, int timeout);//��λs
//�������
static int pollClear(EventLoop* eventloop);

Dispathcer pollDispatchcer = {
	pollInit,
	pollAdd,
	pollremove,
	pollModify,
	pollDispatch,
	pollClear
};

//init  ��ʼ��epoll,poll,select��Ҫ�����ݿ�
static void* pollInit() {
	struct pollData* t = (struct pollData*)malloc(sizeof(struct pollData));
	t->maxfd=0;
	for (int i = 0; i < MAX; i++) {
		t->fds->fd = -1;
		t->fds->events = 0;
		t->fds->revents =0;
	}
	return t;
}
//���
static int pollAdd(Channel* channel, EventLoop* eventloop) {
	struct pollData* data = (struct pollData*)eventloop->data;
	int events = 0;
	if (channel->events & ReadEvent) {//���¼�
		events |= POLLIN;
	}
	if (channel->events & WriteEvent) {//д�¼�
		events |= POLLOUT;
	}
	int i = 0;
	for (; i < MAX; i++) {
		if (data->fds[i].fd== -1) {
			data->fds[i].fd = channel->fd;
			data->fds[i].events = events;
			data->maxfd = data->maxfd > i?data->maxfd:i;
			break;
		}
	}
	if (i >= MAX) {//����װ����
		return -1;
	}
	return 0;
}
//ɾ��
static int pollremove(Channel* channel, EventLoop* eventloop) {
	struct pollData* data = (struct pollData*)eventloop->data;
	int i = 0;
	for (; i < MAX; i++) {
		if (data->fds[i].fd== channel->fd) {
			data->fds[i].fd = -1;
			data->fds[i].events = 0;
			data->fds[i].revents = 0;
		}
	}
	channel->destoryCallback(channel->arg);
	if (i == MAX) return -1;//û�ҵ�
	return 0;
}
//�޸�
static int pollModify(Channel* channel, EventLoop* eventloop) {
	struct pollData* data = (struct pollData*)eventloop->data;
	int events = 0;
	if (channel->events & ReadEvent) {//���¼�
		events |= POLLIN;
	}
	if (channel->events & WriteEvent) {//д�¼�
		events |= POLLOUT;
	}
	int i = 0;
	for (; i <= data->maxfd; i++) {
		if (data->fds[i].fd == channel->fd) {
			data->fds[i].events = events;
			break;
		}
	}
	if (i >= MAX) return -1;//û�ҵ�
	return 0;
}
// �¼����
//timeout ��λs
static int pollDispatch(EventLoop* eventloop, int timeout) {
	struct pollData* data = (struct pollData*)eventloop->data;
	int ret = poll(data->fds, data->maxfd + 1, timeout * 1000);
	if (ret == -1) {
		perror("poll");
		exit(0);
	}
	for (int i = 0; i <= data->maxfd; i++) {
		if (data->fds[i].fd != -1) {
			if (data->fds[i].revents & POLLIN) {//���¼�����
				eventActive(eventloop, data->fds[i].fd, ReadEvent);
			}
			if (data->fds[i].revents & POLLOUT) {//д�¼�����
				eventActive(eventloop, data->fds[i].fd, WriteEvent);
			}
		}
	}
	return 0;
}
//�������
static int pollClear(EventLoop* eventloop) {
	struct pollData* data = (struct pollData*)eventloop->data;
	free(data);
	return 0;
}