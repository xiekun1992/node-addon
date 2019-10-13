#pragma once
#include "common.h"

struct FrameList {
	//AVFrame* frame;
	uint8_t* frame = NULL;
	int size = 0;
	int pts = 0;
	struct FrameList* next;
};

class PictureQueue {
private:
	FrameList* first = NULL;
	FrameList* last = NULL;
public:
	PictureQueue() {}
	~PictureQueue() {}

	int length = 0, bufferSize = 0;
	void init(int bufferSize, int queueLength);
	int getEmptyFrame(uint8_t** frame, int size, int pts);
	int getReallocEmptyFrame(uint8_t** frame, int size, int pts);
	int getDecodedFrame(uint8_t** frame, int* size, int* pts);
	int avail();
	void freeQueue();
	void resetLength();
	int clearQueue();
};