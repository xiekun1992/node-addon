extern "C" {
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
}

namespace utils {
  typedef struct PacketQueue {
    AVPacketList *firstPkt, *lastPkt;
    int packets;
    int size;
  } PacketQueue;

  void packetQueueInit(PacketQueue* q);
  int packetQueuePut(PacketQueue* q, AVPacket* pkt);
  int packetQueueGet(PacketQueue* q, AVPacket* pkt);
}