#include "picture_queue.h"

//void PictureQueue::init() {
//	// 创建长度为30的循环链表
//	length = 30;
//	FrameList* frameList = static_cast<FrameList*> (malloc(sizeof(FrameList)));
//	if (frameList) {
//		frameList->frame = av_frame_alloc();
//		first = last = frameList;
//	}
//	for (int i = 1; i < length; i++) {
//		FrameList* frameList = static_cast<FrameList*> (malloc(sizeof(FrameList)));
//		if (frameList) {
//			frameList->frame = av_frame_alloc();
//			last->next = frameList;
//			last = frameList;
//		}
//	}
//
//	last->next = first;
//}
//// 帧数据转换用
//AVFrame* PictureQueue::getDecodedFrame() {
//	if (last->next != first) {
//		AVFrame* frame = last->next->frame;
//		last = last->next;
//		return frame;
//	}
//	return NULL;
//}
//// 解码线程放置帧数据用
//AVFrame* PictureQueue::getEmptyFrame() {
//	if (first != last) {
//		AVFrame* frame = first->frame;
//		first = first->next;
//		return frame;
//	}
//	return NULL;
//}
//int PictureQueue::avail() {
//	return first == last ? 0 : 1;
//}

void PictureQueue::init(int bufferSize, int queueLength) {
	// 创建长度为30的循环链表
	length = queueLength;
	this->bufferSize = bufferSize;
	FrameList* frameList = static_cast<FrameList*> (malloc(sizeof(FrameList)));
	if (frameList) {
		frameList->frame = static_cast<uint8_t*> (av_malloc(bufferSize));
		first = last = frameList;
	}
	for (int i = 1; i < length; i++) {
		FrameList* frameList = static_cast<FrameList*> (malloc(sizeof(FrameList)));
		if (frameList) {
			frameList->frame = static_cast<uint8_t*> (av_malloc(bufferSize));
			last->next = frameList;
			last = frameList;
		}
	}

	last->next = first;
}
// 帧数据转换用
int PictureQueue::getDecodedFrame(uint8_t** frame, int* size) {
	if (last->next != first) {
		*frame = last->next->frame;
		*size = last->next->size;
		last = last->next;
		return 1;
	}
	return -1;
}
// 解码线程放置帧数据用
int PictureQueue::getEmptyFrame(uint8_t** frame, int size) {
	if (first != last) {
		*frame = first->frame;
		first->size = size;
		first = first->next;
		return 1;
	}
	return -1;
}
int PictureQueue::getReallocEmptyFrame(uint8_t** frame, int size) {
	if (first != last) {
		av_free(first->frame);
		first->frame = static_cast<uint8_t*> (av_malloc(bufferSize));
		*frame = first->frame;
		first->size = size;
		first = first->next;
		return 1;
	}
	return -1;
}

int PictureQueue::avail() {
	return first == last ? 0 : 1;
}