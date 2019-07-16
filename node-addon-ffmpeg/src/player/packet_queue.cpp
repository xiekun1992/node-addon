#include "packet_queue.h"

void PacketQueue::init() {
	size = 0;
	packets = 0;
	first = NULL;
	last = NULL;
}

int PacketQueue::get(AVPacket* pkt) {
	AVPacketList* head = first;
	if (head) {
		first = first->next;
		if (!first) {
			last = NULL;
		}
		--packets;
		size -= head->pkt.size;

		*pkt = head->pkt;
		av_free(head);
		return 0;
	}
	return -1;
}
int PacketQueue::put(AVPacket* pkt) {
	AVPacketList* pktList = static_cast<AVPacketList*> (av_malloc(sizeof(AVPacketList)));
	if (!pktList) {
		return -1;
		//throw runtime_error(string("malloc pkt list failed"));
	}
	pktList->pkt = *pkt;
	pktList->next = NULL;

	if (!last) {
		first = pktList;
	}
	else {
		last->next = pktList;
	}
	last = pktList;
	++packets;
	size += pktList->pkt.size;
	return 0;
}