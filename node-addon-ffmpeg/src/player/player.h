#pragma once
#include "common.h"
#include "packet_queue.h"
#include "picture_queue.h"

#define MAX_AUDIO_FRAME_SIZE 192000 // 1 second of 48khz 32bit audio

class Player {

public:
	Player();
	~Player();


	void init(const char* filename);
	map<string, map<string, string>> getInfo();
	void readAudioPacketThread();
	void readVideoPacketThread();
	void readPacket();
	int decodeAudio();
	int decodeVideo();
	void updateAudioClock(int timeDelta);
	int readyToPlay();

	const char* filename = NULL;
	uint8_t* audioBuffer = NULL;// ÿ�ν�������Ƶ����
	int audioBufferSize = 0;
	int videoBufferSize = 0;
	int audioClock = 0;// ��Ƶʱ�ӱ���ʱ��΢��ֵ
	uint8_t* buffer = NULL;

private:
	AVFormatContext* audioFmtCtx = NULL;
	PictureQueue audioQueue;
	AVCodecContext* audioCodecCtx = NULL;
	AVCodec* audioCodec = NULL;
	int audioStreamIndex = -1;
	struct SwrContext* audioConvertCtx = NULL;

	AVFormatContext* videoFmtCtx = NULL;
	PictureQueue videoQueue;
	AVCodecContext* videoCodecCtx = NULL;
	AVCodec* videoCodec = NULL;
	int videoStreamIndex = -1;
	
	//AVFrame* videoFrame = NULL;
	AVFrame* videoFrameRGB = NULL;
	struct SwsContext* swsCtx = NULL;
};