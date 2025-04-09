#include "ChannelMap.h"
#include<stdbool.h>
#include<stdlib.h>
//³õÊ¼»¯
ChannelMap* initChannelMap(int size) {
	ChannelMap* map = (ChannelMap*)malloc(sizeof(ChannelMap));
	map->list = (Channel**)malloc(sizeof(Channel*) * size);
	map->size = size;
	return map;
}

//Çå¿Õmap
void ChannelMapClear(ChannelMap* map) {
	if (map != NULL) {
		for (int i = 0; i < map->size; i++) {
			if (map->list[i] != NULL) {
				free(map->list[i]);
			}
		}
		free(map->list);
		map->list = NULL;
	}
	map->size = 0;
}
//listÀ©ÈÝ
bool MakeMapRoom(ChannelMap* map, int newSize, int unitSize) {
	if (map->size < newSize) {
		int cursize = map->size;
		while (cursize < newSize) {
			cursize *= 2;
		}
		//À©ÈÝrelloc
		Channel** temp=realloc(map->list, cursize * unitSize);;
		if (temp == NULL) {
			return false;
		}
		else {
			map->list = temp;
			//À©ÈÝ¿Õ¼ä³õÊ¼»¯
			memset(&map->list[map->size],0,(cursize-map->size)*unitSize);
			map->size = cursize;
		}
	}
	return true;
}