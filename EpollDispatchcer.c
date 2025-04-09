#include <sys/epoll.h>
#include"Dispathcer.h"
#include"EventLoop.h"
#include<string.h>
typedef struct EventLoop EventLoop;
#define MAX 520
struct epollData {
	int epfd;
	struct epoll_event* event;
};

//init  ��ʼ��epoll,poll,select��Ҫ�����ݿ�
static void* epollInit();
//���
static int epollAdd(Channel* channel, EventLoop* eventloop);
//ɾ��
static int epollremove(Channel* channel, EventLoop* eventloop);
//�޸�
static int epollModify(Channel* channel, EventLoop* eventloop);
// �¼����
static int epollDispatch(EventLoop* eventloop, int timeout);//��λs
//�������
static int epollClear(EventLoop* eventloop);

Dispathcer epollDispatchcer = {
	epollInit,
	epollAdd,
	epollremove,
	epollModify,
	epollDispatch,
	epollClear
};

//init  ��ʼ��epoll,poll,select��Ҫ�����ݿ�
static void* epollInit() {
	struct epollData* t = (struct epollData*)malloc(sizeof(struct epollData));
	t->epfd = epoll_create(1);
	if (t->epfd == -1) {
		perror("epoll create");
		exit(0);
	}
	t->event = (struct epoll_event*)calloc(MAX,sizeof(struct epoll_event));
	return t;
}
//���
static int epollAdd(Channel* channel, EventLoop* eventloop) {
	struct epollData* data = (struct epollData*)eventloop->data;
	struct epoll_event ev;
	int events = 0;
	if (channel->events & ReadEvent) {//���¼�
		events |= EPOLLIN;
	}
	if (channel->events & WriteEvent) {//д�¼�
		events |= EPOLLOUT;
	}
	ev.events = events;
	ev.data.fd = channel->fd;
	int ret = epoll_ctl(data->epfd, EPOLL_CTL_ADD, channel->fd, &ev);
	return 0;
}
//ɾ��
static int epollremove(Channel* channel, EventLoop* eventloop) {
	struct epollData* data = (struct epollData*)eventloop->data;
	int ret = epoll_ctl(data->epfd, EPOLL_CTL_DEL, channel->fd, NULL);
	channel->destoryCallback(channel->arg);
	return 0;
}
//�޸�
static int epollModify(Channel* channel, EventLoop* eventloop) {
	struct epollData* data = (struct epollData*)eventloop->data;
	struct epoll_event ev;
	int events = 0;
	if (channel->events & ReadEvent) {//���¼�
		events |= EPOLLIN;
	}
	if (channel->events & WriteEvent) {//д�¼�
		events |= EPOLLOUT;
	}
	ev.events = events;
	int ret = epoll_ctl(data->epfd, EPOLL_CTL_MOD, channel->fd, &ev);
	return 0;

}
// �¼����
//timeout ��λs
static int epollDispatch(EventLoop* eventloop, int timeout) {
	if(strcmp(eventloop->threadName,"mainthread")==0) printf("mainDispatch...begin listen......\n");
	else printf("sonDispatch...begin listen......\n");
	struct epollData* data = (struct epollData*)eventloop->data;
	int count=epoll_wait(data->epfd,data->event,MAX,timeout*1000);
	printf("count...%d...things need to do...\n",count);
	for (int i = 0; i < count; i++) {
		int events = data->event[i].events;
		int fd = data->event[i].data.fd;
		//if (events & EPOLLERR || events & EPOLLHUP) {//�����쳣
		//	//ɾ������������
		//	//epollremove(channel, eventloop);
		//	continue;
		//}
		if (events & EPOLLIN) {//���¼�����
			printf("readthing to do\n");
			eventActive(eventloop,data->event[i].data.fd, ReadEvent);
		}
		if (events & EPOLLOUT) {//д�¼�����
			printf("writething to do\n");
			eventActive(eventloop, data->event[i].data.fd, WriteEvent);
		}
	}
	return 0;
}
//�������
static int epollClear(EventLoop* eventloop) {
	struct epollData* data = (struct epollData*)eventloop->data;
	free(data->event);
	free(data);
	return 0;
}