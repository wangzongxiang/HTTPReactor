#include "WorkThread.h"
#include"EventLoop.h"
typedef struct  EventLoop EventLoop;
//��ʼ��
int initWorkThread(WorkThread* thread, int index) {//intdexΪ�̳߳ص��±�
	thread->threadID = 0;//��ȡ�߳�ID
	thread->eventloop = NULL;//��Ӧ�ѳ�ʼ��
	pthread_mutex_init(&thread->mutex,NULL);//��������ʼ��
	pthread_cond_init(&thread->cood, NULL);//����������ʼ��
	sprintf(thread->name, "subthread%d", index);//�߳���
	printf("initWorkThread--%d...finish\n", index);
	return 0;
}
//���̵߳Ļص�����
void *callBack(void* arg) {
	WorkThread* thread = (WorkThread*)arg;
	pthread_mutex_lock(&thread->mutex);
	thread->eventloop = EventLoopInitN(thread->name);
	thread->threadID = pthread_self();
	pthread_mutex_unlock(&thread->mutex);
	pthread_cond_signal(&thread->cood);//������������
	EventLoopRun(thread->eventloop);
	printf("workThreadRun...finish...\n");
	return NULL;
}
//�����߳�
int workThreadRun(WorkThread* thread) {
	int ret=pthread_create(&thread->threadID, NULL, callBack,thread);
	pthread_mutex_lock(&thread->mutex);
	if (ret == -1) {
		perror("workThread_create");
		return -1;
	}
	//�����߳���������ֹ���߳�eventloopû����ɴ���
	while (thread->eventloop == NULL) {
		pthread_cond_wait(&thread->cood,&thread->mutex);
	}
	pthread_mutex_unlock(&thread->mutex);
	return 0;
}
