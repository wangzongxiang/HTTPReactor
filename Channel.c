#include "Channel.h"
#include<fcntl.h>
#include<unistd.h>
Channel* channelInit(int fd, int events, handleFunc readCallback, handleFunc writeCallback, handleFunc destoryCallback, void* arg) {
	printf("channelInit...\n");
	Channel* t = (Channel*)malloc(sizeof(Channel));
	t->arg = arg;
	t->events = events;
	t->fd = fd;
	t->readCallback = readCallback;
	t->writeCallback = writeCallback;
	t->destoryCallback = destoryCallback;
	return t;
}

void  channelDestory(Channel* channel,EventLoop* eventLoop) {
	eventLoop->channelmap->list[channel->fd] = NULL;
	close(channel->fd);
	free(channel);
}
void writeEventEnable(Channel* channel, bool flag) {
	if (flag) {//×·¼ÓÐ´ÊôÐÔ
		channel->events |= WriteEvent;
	}
	else {//É¾³ýÐ´ÊôÐÔ
		channel->events ^= WriteEvent;
	}
}
bool isWriteEventEnable(Channel* channel) {
	return channel->events & WriteEvent;
}