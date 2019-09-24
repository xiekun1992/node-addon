#include "picture_queue.h"

//void PictureQueue::init() {
//	// ��������Ϊ30��ѭ������
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
//// ֡����ת����
//AVFrame* PictureQueue::getDecodedFrame() {
//	if (last->next != first) {
//		AVFrame* frame = last->next->frame;
//		last = last->next;
//		return frame;
//	}
//	return NULL;
//}
//// �����̷߳���֡������
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
	// ��������Ϊ30��ѭ������
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
// ֡����ת����
int PictureQueue::getDecodedFrame(uint8_t** frame, int* size, int* pts) {
	if (last->next != first) {
		FrameList* frameList = last->next;
		if (frameList->pts > *pts) {
			return -1;
			//while (*pts > -1) {
			//	if (*pts >= frameList->pts || last->next == first) {
			//		break;
			//	}
			//	else {
			//		frameList = last->next;
			//	}
			//}
		}
		if (frameList != NULL) {
			*frame = frameList->frame;
			*size = frameList->size;
			//*pts = frameList->pts;
			last = frameList;
			return 1;
		}
	}
	return -1;
}
// �����̷߳���֡������
int PictureQueue::getEmptyFrame(uint8_t** frame, int size, int pts) {
	if (first != last) {
		//printf("%d\n", pts);
		*frame = first->frame;
		first->size = size;
		first->pts = pts;
		first = first->next;
		return 1;
	}
	return -1;
}
int PictureQueue::getReallocEmptyFrame(uint8_t** frame, int size, int pts) {
	if (first != last) {
		av_free(first->frame);
		first->frame = static_cast<uint8_t*> (av_malloc(bufferSize));
		*frame = first->frame;
		first->size = size;
		first->pts = pts;
		first = first->next;
		return 1;
	}
	return -1;
}

int PictureQueue::avail() {
	return (first == last && first != NULL) ? 0 : 1;
}
void PictureQueue::freeQueue() {
	if (last != NULL && first != NULL) {
		FrameList* tmp = last->next;

		while (tmp != last) {
			first = tmp;
			tmp = tmp->next;
			first->next = NULL;
			av_free(first->frame);
			free(first);
		}
		tmp->next = NULL;
		av_free(tmp->frame);
		free(tmp);
	}
	last = first = NULL;
	length = 0;
	bufferSize = 0;
}
void PictureQueue::resetLength() {
	first = last->next;
}