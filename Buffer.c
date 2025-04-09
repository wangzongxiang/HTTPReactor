#include "Buffer.h"
#include<unistd.h>
#include<sys/socket.h>
#include<string.h>
//buffer��ʼ��
Buffer* initBuffer(int capacity) {
	Buffer* buffer = (Buffer*)malloc(sizeof(Buffer));
	buffer->readPos = 0;
	buffer->writePos = 0;
	buffer->capacity = capacity;
	buffer->data=(char*)malloc(capacity);
	memset(buffer->data,0,capacity);
	return buffer;
}

void bufferDestory(Buffer* buffer) {
	if (buffer != NULL && buffer->data != NULL) {
		free(buffer->data);
		free(buffer);
	}
}

void bufferExtend(Buffer* buffer, int newSize) {
	
	if (buffer->capacity - buffer->writePos > newSize) {//�ڴ湻��
		return;//����Ҫ����
	}
	else if (buffer->capacity - buffer->writePos + buffer->readPos - (int)buffer->data > newSize) {//�ڴ湻�ã�������Ƭ����Ҫ�ϲ��Ź���  buffer->capacity - buffer->writePos + buffer->readPos - buffer->data > newSize
		//�õ�δ���ڴ�Ĵ�С
		int size = buffer->writePos - buffer->readPos;
		//�ƶ��ڴ�
		memcpy(buffer->data,buffer->data+buffer->readPos,size);
		buffer->readPos = 0;
		buffer->writePos = size;
	}
	else {	//�ڴ治����  ����
		void* temp=realloc(buffer->data,buffer->capacity+ newSize);
		if (temp == NULL) return;
		memset(temp+buffer->capacity,0, newSize);//�������ݵĿռ���г�ʼ��
		buffer->data = temp;
		buffer->capacity += newSize;
	}
}

//д����
int bufferAppendData(Buffer* buffer, char* data, int size) {
	if (buffer == NULL || data == NULL || size <= 0) return -1;
	bufferExtend(buffer, size);//ȷ���ռ乻д
	memcpy(buffer->data + buffer->writePos, data, size);//���ݿ���
	buffer->writePos += size;//����
	return 0;
}
int bufferAppendString(Buffer* buffer, char* data) {
	bufferAppendData(buffer, data, strlen(data));
	return 0;
}

int bufferSocketRead(Buffer* buffer, int fd){
	int n = 3;
	struct iovec vec[n];
	//��ʼ������
	vec[0].iov_base = buffer->data + buffer->readPos;
	vec[0].iov_len = buffer->capacity - buffer->writePos;
	for (int i = 1; i < n; i++) {
		vec[i].iov_base = (char*)malloc(40960);
		vec[i].iov_len = 40960;
	}
	int res = readv(fd,vec,n);
	int temp=res;
	if (res < 0) return -1;
	else if (res <=buffer->capacity - buffer->writePos) {//û��ʹ�õ���������
		buffer->writePos += res;//����дָ��
	}
	else {//���������ݿ�����buffer��
		for (int i = 0; i < n&&res>0; i++) {
			if (res >= vec[i].iov_len) {
				bufferAppendData(buffer, vec[i].iov_base, vec[i].iov_len);
				res -= vec[i].iov_len;
			}
			else {
				bufferAppendData(buffer, vec[i].iov_base,res);
				break;
			}
		}
	}
	for (int i = 1; i < n; i++) {
		free(vec[i].iov_base);
	}
	return temp;
}

char* bufferFindCRLF(Buffer* buffer) {//��ȡ��β��ַ
	//printf("%s", buffer->data + buffer->readPos);
	return strstr(buffer->data + buffer->readPos, "\r\n");
	//return memmem(buffer->data+buffer->readPos,buffer->writePos-buffer->readPos,"\r\n",2);
}

//��������
int bufferSendData(Buffer* buffer, int fd) {
	int size=buffer->writePos - buffer->readPos;
	if (size == 0) return -1;//û��������Ҫ����

	int count = send(fd, buffer->data + buffer->readPos,size,0);
	buffer->readPos += count;
	usleep(1);
	return 0;
}