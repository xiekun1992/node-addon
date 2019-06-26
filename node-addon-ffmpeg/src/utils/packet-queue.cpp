#include "packet-queue.h"

void utils::packetQueueInit(PacketQueue* q) {
  memset(q, 0, sizeof(PacketQueue));
}
int utils::packetQueuePut(PacketQueue* q, AVPacket* pkt) {
  AVPacketList* pkt1 = (AVPacketList*)av_malloc(sizeof(AVPacketList));
  if (!pkt1) {
    return -1;
  }
  pkt1->pkt = *pkt;
  pkt1->next = NULL;

  if (!q->lastPkt) {
    q->firstPkt = pkt1;
  } else {
    q->lastPkt->next = pkt1;
  }
  q->lastPkt = pkt1;
  q->packets++;
  q->size += pkt1->pkt.size;
  return 0;
}
int utils::packetQueueGet(PacketQueue* q, AVPacket* pkt) {
  AVPacketList* pkt1 = q->firstPkt;
  if (pkt1) {
    q->firstPkt = pkt1->next;
    if (!q->firstPkt) { //只有一个元素
      q->lastPkt = NULL;
    }
    q->packets--;
    q->size -= pkt1->pkt.size;
    *pkt = pkt1->pkt;
    av_free(pkt1);
    return 0;
  } else {
    return -1;
  }
}