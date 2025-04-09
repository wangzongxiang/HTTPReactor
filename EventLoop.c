#include"EventLoop.h"
#include<assert.h>
#include <unistd.h>
#include<sys/socket.h>
#include"Channel.h"
#include"TcpConnection.h"
#include<stdlib.h>
//写数据
void TaskWakeUp(EventLoop* eventloop) {
	printf("wakeUp sonThread...\n");
	const char* msg = "1234";
	write(eventloop->socketPair[0], msg, strlen(msg));
}

//读数据
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
	if (threadName == NULL) {//线程名字赋值
		strcpy(eventloop->threadName, "mainthread");
	}
	else {
		strcpy(eventloop->threadName, threadName);
	}
	eventloop->dispatcher = &epollDispatchcer;//该例子中使用epoll模型
	eventloop->data = eventloop->dispatcher->init();
	eventloop->head = NULL;
	eventloop->tail = NULL;
	eventloop->channelmap = initChannelMap(128);//默认元素为128
	int ret = socketpair(AF_UNIX, SOCK_STREAM, 0, eventloop->socketPair);
	if (ret == -1) {
		perror("socketpair error:");
		exit(0);
	}
	//为了能让主线程唤醒堵塞的子线程   指定socketPair下标0发数据，1收数据
	printf("eventLoop...Channel...\n");
	Channel* channel = channelInit(eventloop->socketPair[1], ReadEvent, readLocalMeg, NULL, NULL, eventloop);
	eventLoopAddTask(channel, eventloop, ADD);
	return eventloop;
}

//启动反应堆
int EventLoopRun(EventLoop* eventloop) {
	assert(eventloop != NULL);//断言  不满足条件直接挂
	//取出事件分发和检测模型
	printf("EventLoopRun...\n");
	Dispathcer* dispathcer = eventloop->dispatcher;
	if (eventloop->threadID != pthread_self()) {//线程ID错误
		perror("threadID_error:");
		exit(0);
	}
	//循环进行事件处理
	while (!eventloop->isQuit) {
		dispathcer->dispatch(eventloop, 2);//超时时长为2s
		eventLoopProcessTask(eventloop);
	}
	return 0;
}

//处理被激活的文件描述符函数
int eventActive(EventLoop* eventloop, int fd, int event) {
	if (eventloop == NULL || fd < 0 || fd>eventloop->channelmap->size) return -1;
	printf("enter the do Function...\n");
	Channel* channel = eventloop->channelmap->list[fd];
	assert(fd == channel->fd);
	if (event & ReadEvent && channel->readCallback != NULL) {//读事件
		printf("begin use readFunction...\n");
		channel->readCallback(channel->arg);
	}
	if (event & WriteEvent && channel->writeCallback != NULL) {//写事件
		printf("ready use writeFunction...\n");
		channel->writeCallback(channel->arg);
	}
	return 0;
}
//向任务队列中添加任务

int eventLoopAddTask(Channel* channel, EventLoop* eventloop, int type) {
	if (strcmp(eventloop->threadName, "mainthread") == 0) printf("mainthread...eventLoopAddTask..add..Channel...\n");
	else  printf("sonthread...eventLoopAddTask..add..Channel...\n");
	ChannelElement* node = (ChannelElement*)malloc(sizeof(ChannelElement));
	node->next = NULL;
	node->channel = channel;
	node->type = type;
	pthread_mutex_lock(&eventloop->mutex);//任务队列可能会有多个线程同时访问 加锁保持线程同步
	if (eventloop->head == NULL) {
		eventloop->tail = node;
		eventloop->head = node;
	}
	else {
		eventloop->tail->next = node;
		eventloop->tail = node;
	}
	pthread_mutex_unlock(&eventloop->mutex);//解锁
	//处理结点
	//每个线程都有eventloop，主线程的任务要交给子线程处理
	if (eventloop->threadID == pthread_self()){//if (eventloop->threadID == pthread_self()) 
		eventLoopProcessTask(eventloop);
		printf("selfThreadEventLoop...\n");
	}
	else {//主线程调用子线程
		//主线程告诉子线程处理
		printf("maineventLoop.......................................................\n");
		TaskWakeUp(eventloop);//唤醒子线程  当向0发送数据时，0会自动向1把数据发送过去
	}
	return 0;
}

//处理队列中的任务
int eventLoopProcessTask(EventLoop* eventloop) {
	if (eventloop->head == NULL) return -1;
	pthread_mutex_lock(&eventloop->mutex);
	while (eventloop->head != NULL) {
		ChannelElement* task = eventloop->head;
		Channel* channel = task->channel;
		if (task->type == ADD) {//添加到
			eventLoopAdd(channel, eventloop);
		}
		else if (task->type == DEL) {//删除
			eventLoopDel(channel, eventloop);
		}
		else//修改
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

//添加文件描述符到用于监测的dispathcer
int eventLoopAdd(Channel* channel, EventLoop* eventloop) {//添加

	if (eventloop->channelmap->size <= channel->fd) {
		if (!MakeMapRoom(eventloop->channelmap, channel->fd, sizeof(Channel*))) {//没有足够的空间扩容
			return -1;
		}
	}
	if (eventloop->channelmap->list[channel->fd] == NULL) {
		eventloop->channelmap->list[channel->fd] = channel;
		eventloop->dispatcher->add(channel, eventloop);
	}
	return 0;
}
//删除文件描述符到用于监测的dispathcer
int eventLoopDel(Channel* channel, EventLoop* eventloop) {
	if (eventloop->channelmap->size <= channel->fd || eventloop->channelmap->list[channel->fd] == NULL) {//没有存储
		return -1;
	}
	eventloop->channelmap->list[channel->fd] = NULL;
	return eventloop->dispatcher->remove(channel, eventloop);
}
//修改文件描述符到用于监测的dispathcer
int eventLoopMod(Channel* channel, EventLoop* eventloop) {
	if (eventloop->channelmap->size <= channel->fd || eventloop->channelmap->list[channel->fd] == NULL) {//没有存储
		return -1;
	}
	return	eventloop->dispatcher->modify(channel, eventloop);
}
