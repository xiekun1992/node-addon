#pragma once
#include "common.h"
class PictureQueue {
private:
	AVFrame* first = NULL;
	AVFrame* last = NULL;
public:
	PictureQueue() {}
	~PictureQueue() {}

	int size = 0, pics = 0;
	void init();
	int get(AVFrame* frame);
	int put(AVFrame* frame);
};