#include "WorkThread.h"
#include"EventLoop.h"
typedef struct  EventLoop EventLoop;
//初始化
int initWorkThread(WorkThread* thread, int index) {//intdex为线程池的下标
	thread->threadID = 0;//获取线程ID
	thread->eventloop = NULL;//反应堆初始化
	pthread_mutex_init(&thread->mutex,NULL);//互斥锁初始化
	pthread_cond_init(&thread->cood, NULL);//条件变量初始化
	sprintf(thread->name, "subthread%d", index);//线程名
	printf("initWorkThread--%d...finish\n", index);
	return 0;
}
//子线程的回调函数
void *callBack(void* arg) {
	WorkThread* thread = (WorkThread*)arg;
	pthread_mutex_lock(&thread->mutex);
	thread->eventloop = EventLoopInitN(thread->name);
	thread->threadID = pthread_self();
	pthread_mutex_unlock(&thread->mutex);
	pthread_cond_signal(&thread->cood);//唤醒条件变量
	EventLoopRun(thread->eventloop);
	printf("workThreadRun...finish...\n");
	return NULL;
}
//启动线程
int workThreadRun(WorkThread* thread) {
	int ret=pthread_create(&thread->threadID, NULL, callBack,thread);
	pthread_mutex_lock(&thread->mutex);
	if (ret == -1) {
		perror("workThread_create");
		return -1;
	}
	//让主线程阻塞，防止子线程eventloop没有完成创建
	while (thread->eventloop == NULL) {
		pthread_cond_wait(&thread->cood,&thread->mutex);
	}
	pthread_mutex_unlock(&thread->mutex);
	return 0;
}
