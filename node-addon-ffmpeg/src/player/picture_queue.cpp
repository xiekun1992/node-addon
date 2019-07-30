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

void PictureQueue::init(int bufferSize) {
	// 创建长度为30的循环链表
	length = 30;
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
uint8_t* PictureQueue::getDecodedFrame() {
	if (last->next != first) {
		uint8_t* frame = last->next->frame;
		last = last->next;
		return frame;
	}
	return NULL;
}
// 解码线程放置帧数据用
uint8_t* PictureQueue::getEmptyFrame() {
	if (first != last) {
		uint8_t* frame = first->frame;
		first = first->next;
		return frame;
	}
	return NULL;
}
int PictureQueue::avail() {
	return first == last ? 0 : 1;
}