#pragma once
#include<stdio.h>
#include<stdlib.h>
#include<sys/uio.h>
#include<string.h>
typedef struct Buffer {
	//指向内存的指针
	char* data;
	int capacity;//容量大小
	int readPos;//读位置
	int writePos;//写位置
}Buffer;


//buffer初始化
Buffer* initBuffer(int capacity);
//销毁
void bufferDestory(Buffer* buffer);
//扩容
void bufferExtend(Buffer* buffer, int newSize);
//写内存 1.直接写  2.接收套接字数据
//直接写
int bufferAppendData(Buffer* buffer,char* data,int size);//当字符串中有特殊字符时
int bufferAppendString(Buffer* buffer, char* data);//正常情况
//套接字
int bufferSocketRead(Buffer* buffer,int fd);//fd 通信的文件描述符
//获取一行数据 /r/n结尾,返回结尾位置
char* bufferFindCRLF(Buffer* buffer);
//发送数据
int bufferSendData(Buffer* buffer,int fd);


