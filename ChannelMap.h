#pragma once
#include"Channel.h"
//�����洢key-value
//key�����±�
//value:��ӦChannleָ��ָ��ĵ�ַ
typedef struct Channel Channel;
typedef struct ChannelMap {
	int size;//����Ĵ�С
	//�洢Channel*������
	Channel** list;//

}ChannelMap;

ChannelMap* initChannelMap(int size);//list�ĳ���
//���map
void ChannelMapClear(ChannelMap* map);
//list����
bool MakeMapRoom(ChannelMap* map,int newSize,int unitSize);
