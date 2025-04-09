#pragma once
#include"Channel.h"
//用来存储key-value
//key数组下标
//value:对应Channle指针指向的地址
typedef struct Channel Channel;
typedef struct ChannelMap {
	int size;//数组的大小
	//存储Channel*的数组
	Channel** list;//

}ChannelMap;

ChannelMap* initChannelMap(int size);//list的长度
//清空map
void ChannelMapClear(ChannelMap* map);
//list扩容
bool MakeMapRoom(ChannelMap* map,int newSize,int unitSize);
