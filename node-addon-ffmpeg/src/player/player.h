#pragma once
#include "common.h"
#include "packet_queue.h"

#define MAX_AUDIO_FRAME_SIZE 192000 // 1 second of 48khz 32bit audio

class Player {

public:
	Player();
	~Player();

	const char* filename = NULL;

	void init(const char* filename);
	map<string, string> getInfo();
	void readAudioPacketThread();
	void readVideoPacketThread();
	void readPacket();
	void decodeAudio();
	void decodeVideo();

private:
	AVFormatContext* audioFmtCtx = NULL;
	AVFormatContext* videoFmtCtx = NULL;

	PacketQueue audioQueue;
	AVCodecContext* audioCodecCtx = NULL;
	AVCodec* audioCodec = NULL;
	int audioStreamIndex = -1;
	struct SwrContext* audioConvertCtx = NULL;
	uint8_t* audioBuffer = NULL;

	PacketQueue videoQueue;
	AVCodecContext* videoCodecCtx = NULL;
	AVCodec* videoCodec = NULL;
	int videoStreamIndex = -1;
	AVFrame* videoFrame = NULL;
	AVFrame* videoFrameRGB = NULL;
	struct SwsContext* swsCtx = NULL;
	uint8_t* buffer = NULL;
};