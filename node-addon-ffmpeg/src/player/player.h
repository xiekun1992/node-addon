#pragma once
#include "common.h"
#include "packet_queue.h"
#include "picture_queue.h"
#include "video_info.h"

#define MAX_AUDIO_FRAME_SIZE 192000 // 1 second of 48khz 32bit audio

class Player {

public:
	Player();
	~Player();


	void init(const char* filename);
	VideoInfo getInfo();
	void readAudioPacketThread();
	void readVideoPacketThread();
	void readPacket();
	int decodeAudio();
	int decodeVideo();
	void updateAudioClock(int timeDelta);
	int readyToPlay();
	void freeMedia();
	void suspendReadThread();
	void resumeReadThread();
	int seek(int timestamp);

	const char* filename = NULL;
	uint8_t* audioBuffer = NULL;// 每次解码后的音频数据
	int audioBufferSize = 0;
	int videoBufferSize = 0;
	int audioClock = 0;// 音频时钟保存时间微秒值
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
	//AVFrame* videoFrameRGB = NULL;
	struct SwsContext* swsCtx = NULL;

	thread *videoThread = NULL;
	thread *audioThread = NULL;
	bool suspend = false;

	int seekTimestamp = 0;
};