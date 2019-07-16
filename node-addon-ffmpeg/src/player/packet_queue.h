#pragma once
#include "common.h"

class PacketQueue {
private:
	AVPacketList* first = NULL;
	AVPacketList* last = NULL;

public:
	PacketQueue() {}
	~PacketQueue() {}
	
	int size = 0, packets = 0;
	void init();
	int get(AVPacket* pkt);
	int put(AVPacket* pkt);

};