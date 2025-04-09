#include"EventLoop.h"
#include<assert.h>
#include <unistd.h>
#include<sys/socket.h>
#include"Channel.h"
#include"TcpConnection.h"
#include<stdlib.h>
//д����
void TaskWakeUp(EventLoop* eventloop) {
	printf("wakeUp sonThread...\n");
	const char* msg = "1234";
	write(eventloop->socketPair[0], msg, strlen(msg));
}

//������
int readLocalMeg(void* arg) {
	EventLoop* eventloop = (EventLoop*)arg;
	char buf[1024];
	read(eventloop->socketPair[1], buf, sizeof(buf));
	return 0;

}


EventLoop* eventLoopInit()
{
	EventLoop* eventloop = EventLoopInitN(NULL);
	return eventloop;
}

EventLoop* EventLoopInitN(char* threadName)
{
	EventLoop* eventloop = (EventLoop*)malloc(sizeof(EventLoop));
	eventloop->isQuit = false;
	eventloop->threadID = pthread_self();
	pthread_mutex_init(&eventloop->mutex, NULL);
	if (threadName == NULL) {//�߳����ָ�ֵ
		strcpy(eventloop->threadName, "mainthread");
	}
	else {
		strcpy(eventloop->threadName, threadName);
	}
	eventloop->dispatcher = &epollDispatchcer;//��������ʹ��epollģ��
	eventloop->data = eventloop->dispatcher->init();
	eventloop->head = NULL;
	eventloop->tail = NULL;
	eventloop->channelmap = initChannelMap(128);//Ĭ��Ԫ��Ϊ128
	int ret = socketpair(AF_UNIX, SOCK_STREAM, 0, eventloop->socketPair);
	if (ret == -1) {
		perror("socketpair error:");
		exit(0);
	}
	//Ϊ���������̻߳��Ѷ��������߳�   ָ��socketPair�±�0�����ݣ�1������
	printf("eventLoop...Channel...\n");
	Channel* channel = channelInit(eventloop->socketPair[1], ReadEvent, readLocalMeg, NULL, NULL, eventloop);
	eventLoopAddTask(channel, eventloop, ADD);
	return eventloop;
}

//������Ӧ��
int EventLoopRun(EventLoop* eventloop) {
	assert(eventloop != NULL);//����  ����������ֱ�ӹ�
	//ȡ���¼��ַ��ͼ��ģ��
	printf("EventLoopRun...\n");
	Dispathcer* dispathcer = eventloop->dispatcher;
	if (eventloop->threadID != pthread_self()) {//�߳�ID����
		perror("threadID_error:");
		exit(0);
	}
	//ѭ�������¼�����
	while (!eventloop->isQuit) {
		dispathcer->dispatch(eventloop, 2);//��ʱʱ��Ϊ2s
		eventLoopProcessTask(eventloop);
	}
	return 0;
}

//����������ļ�����������
int eventActive(EventLoop* eventloop, int fd, int event) {
	if (eventloop == NULL || fd < 0 || fd>eventloop->channelmap->size) return -1;
	printf("enter the do Function...\n");
	Channel* channel = eventloop->channelmap->list[fd];
	assert(fd == channel->fd);
	if (event & ReadEvent && channel->readCallback != NULL) {//���¼�
		printf("begin use readFunction...\n");
		channel->readCallback(channel->arg);
	}
	if (event & WriteEvent && channel->writeCallback != NULL) {//д�¼�
		printf("ready use writeFunction...\n");
		channel->writeCallback(channel->arg);
	}
	return 0;
}
//������������������

int eventLoopAddTask(Channel* channel, EventLoop* eventloop, int type) {
	if (strcmp(eventloop->threadName, "mainthread") == 0) printf("mainthread...eventLoopAddTask..add..Channel...\n");
	else  printf("sonthread...eventLoopAddTask..add..Channel...\n");
	ChannelElement* node = (ChannelElement*)malloc(sizeof(ChannelElement));
	node->next = NULL;
	node->channel = channel;
	node->type = type;
	pthread_mutex_lock(&eventloop->mutex);//������п��ܻ��ж���߳�ͬʱ���� ���������߳�ͬ��
	if (eventloop->head == NULL) {
		eventloop->tail = node;
		eventloop->head = node;
	}
	else {
		eventloop->tail->next = node;
		eventloop->tail = node;
	}
	pthread_mutex_unlock(&eventloop->mutex);//����
	//������
	//ÿ���̶߳���eventloop�����̵߳�����Ҫ�������̴߳���
	if (eventloop->threadID == pthread_self()){//if (eventloop->threadID == pthread_self()) 
		eventLoopProcessTask(eventloop);
		printf("selfThreadEventLoop...\n");
	}
	else {//���̵߳������߳�
		//���̸߳������̴߳���
		printf("maineventLoop.......................................................\n");
		TaskWakeUp(eventloop);//�������߳�  ����0��������ʱ��0���Զ���1�����ݷ��͹�ȥ
	}
	return 0;
}

//��������е�����
int eventLoopProcessTask(EventLoop* eventloop) {
	if (eventloop->head == NULL) return -1;
	pthread_mutex_lock(&eventloop->mutex);
	while (eventloop->head != NULL) {
		ChannelElement* task = eventloop->head;
		Channel* channel = task->channel;
		if (task->type == ADD) {//��ӵ�
			eventLoopAdd(channel, eventloop);
		}
		else if (task->type == DEL) {//ɾ��
			eventLoopDel(channel, eventloop);
		}
		else//�޸�
		{
			eventLoopMod(channel, eventloop);
		}
		eventloop->head = eventloop->head->next;
		free(task);
	}
	eventloop->tail = NULL;
	pthread_mutex_unlock(&eventloop->mutex);
	return 0;
}

//����ļ������������ڼ���dispathcer
int eventLoopAdd(Channel* channel, EventLoop* eventloop) {//���

	if (eventloop->channelmap->size <= channel->fd) {
		if (!MakeMapRoom(eventloop->channelmap, channel->fd, sizeof(Channel*))) {//û���㹻�Ŀռ�����
			return -1;
		}
	}
	if (eventloop->channelmap->list[channel->fd] == NULL) {
		eventloop->channelmap->list[channel->fd] = channel;
		eventloop->dispatcher->add(channel, eventloop);
	}
	return 0;
}
//ɾ���ļ������������ڼ���dispathcer
int eventLoopDel(Channel* channel, EventLoop* eventloop) {
	if (eventloop->channelmap->size <= channel->fd || eventloop->channelmap->list[channel->fd] == NULL) {//û�д洢
		return -1;
	}
	eventloop->channelmap->list[channel->fd] = NULL;
	return eventloop->dispatcher->remove(channel, eventloop);
}
//�޸��ļ������������ڼ���dispathcer
int eventLoopMod(Channel* channel, EventLoop* eventloop) {
	if (eventloop->channelmap->size <= channel->fd || eventloop->channelmap->list[channel->fd] == NULL) {//û�д洢
		return -1;
	}
	return	eventloop->dispatcher->modify(channel, eventloop);
}
