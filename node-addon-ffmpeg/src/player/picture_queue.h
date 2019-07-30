#pragma once
#include "common.h"

//struct FrameList {
//	AVFrame* frame;
//	struct FrameList* next;
//};

//class PictureQueue {
//private:
//	FrameList* first = NULL;
//	FrameList* last = NULL;
//public:
//	PictureQueue() {}
//	~PictureQueue() {}
//
//	int length = 0;
//	void init();
//	AVFrame* getEmptyFrame();
//	AVFrame* getDecodedFrame();
//	int avail();
//};


struct FrameList {
	//AVFrame* frame;
	uint8_t* frame = NULL;
	struct FrameList* next;
};

class PictureQueue {
private:
	FrameList* first = NULL;
	FrameList* last = NULL;
public:
	PictureQueue() {}
	~PictureQueue() {}

	int length = 0;
	void init(int bufferSize);
	uint8_t* getEmptyFrame();
	uint8_t* getDecodedFrame();
	int avail();
};