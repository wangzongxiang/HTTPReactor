#pragma once
#include<stdio.h>
#include<stdlib.h>
#include<sys/uio.h>
#include<string.h>
typedef struct Buffer {
	//ָ���ڴ��ָ��
	char* data;
	int capacity;//������С
	int readPos;//��λ��
	int writePos;//дλ��
}Buffer;


//buffer��ʼ��
Buffer* initBuffer(int capacity);
//����
void bufferDestory(Buffer* buffer);
//����
void bufferExtend(Buffer* buffer, int newSize);
//д�ڴ� 1.ֱ��д  2.�����׽�������
//ֱ��д
int bufferAppendData(Buffer* buffer,char* data,int size);//���ַ������������ַ�ʱ
int bufferAppendString(Buffer* buffer, char* data);//�������
//�׽���
int bufferSocketRead(Buffer* buffer,int fd);//fd ͨ�ŵ��ļ�������
//��ȡһ������ /r/n��β,���ؽ�βλ��
char* bufferFindCRLF(Buffer* buffer);
//��������
int bufferSendData(Buffer* buffer,int fd);


