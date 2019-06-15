#include "ffmpeg-video.h"

AVFormatContext* pFormatCtx = NULL;
const char* filename = "D:\\Wildlife.wmv";
AVCodecContext* pCodecCtxOrig = NULL;
AVCodecContext* pCodecCtx = NULL;
int videoStream = -1;
AVCodec* pCodec = NULL;
uint8_t* buffer = NULL;
AVFrame* pFrame = NULL;
AVFrame* pFrameRGB = NULL;
struct SwsContext* sws_ctx = NULL;
int numBytes = 0;

void ffmpeg::SaveFrame(AVFrame* pFrame, int width, int height, int iFrame) {
	FILE* pFile;
	char szFilename[32];
	int y;
	sprintf(szFilename, "D:\\frame%d.ppm", iFrame);
	pFile = fopen(szFilename, "wb");
	if (!pFile) {
		return;
	}
	fprintf(pFile, "P6\n%d %d\n255\n", width, height);
	for (y = 0; y < height; y++) {
		fwrite(pFrame->data[0] + y * pFrame->linesize[0], 1, width * 3, pFile);
	}
	fclose(pFile);
}

uint8_t* ffmpeg::extractRGB(AVFrame* frame, int width, int height) {
	uint8_t* buf = (uint8_t*)malloc(sizeof(uint8_t) * width * 3 * height);
	for (int y = 0; y < height; y++) {
		for (int x = 0; x < width * 3; x++) {
			buf[y * width * 3 + x] = *(frame->data[0] + y * frame->linesize[0] + x);
			// int p = x*3+y*frame->linesize[0];
			// r = frame->data[0][p];
			// g = frame->data[0][p+1];
			// b = frame->data[0][p+2];
		}
	}
	return buf;
}

bool ffmpeg::config() {
	if (avformat_open_input(&pFormatCtx, filename, NULL, NULL) < 0) {
		return false;
	}
	if (avformat_find_stream_info(pFormatCtx, NULL) < 0) {
		return false;
	}
	for (unsigned int i = 0; i < pFormatCtx->nb_streams; i++) {
		if (pFormatCtx->streams[i]->codecpar->codec_type == AVMEDIA_TYPE_VIDEO) {
			videoStream = i;
			break;
		}
	}
	if (videoStream == -1) {
		return false;
	}
	// // printf("11111\n");
	pCodecCtx = pFormatCtx->streams[videoStream]->codec;
	// pCodecCtx = avcodec_alloc_context3(NULL);
	if (avcodec_parameters_to_context(pCodecCtx, pFormatCtx->streams[videoStream]->codecpar) < 0) {
		return false;
	}
	pCodec = avcodec_find_decoder(pCodecCtx->codec_id);
	if (!pCodec) {
		return false;
	}
	// if (avcodec_copy_context(pCodecCtx, pCodecCtxOrig) != 0) {
	// 	return false;
	// }
	if (avcodec_open2(pCodecCtx, pCodec, NULL) < 0) {
		return false;
	}
	pFrame = av_frame_alloc();
	if (!pFrame) {
		return false;
	}
	pFrameRGB = av_frame_alloc();
	if (!pFrameRGB) {
		return false;
	}

	numBytes = avpicture_get_size(AV_PIX_FMT_RGB24, pCodecCtx->width, pCodecCtx->height);
	buffer = (uint8_t*)av_malloc(numBytes * sizeof(uint8_t));

	avpicture_fill((AVPicture*)pFrameRGB, buffer, AV_PIX_FMT_RGB24, pCodecCtx->width, pCodecCtx->height);
	sws_ctx = sws_getContext(pCodecCtx->width, pCodecCtx->height, pCodecCtx->pix_fmt, pCodecCtx->width, pCodecCtx->height, AV_PIX_FMT_RGB24, SWS_BILINEAR, NULL, NULL, NULL);

	return true;
}

uint8_t* ffmpeg::extractFrame() {
	int frameFinished;
	AVPacket packet;
	while (av_read_frame(pFormatCtx, &packet) >= 0) {
		if (packet.stream_index == videoStream) {
			avcodec_decode_video2(pCodecCtx, pFrame, &frameFinished, &packet);
			if (frameFinished) {
				sws_scale(sws_ctx, (uint8_t const* const*)pFrame->data, pFrame->linesize, 0, pCodecCtx->height, pFrameRGB->data, pFrameRGB->linesize);

					av_free_packet(&packet);
					break;
			}
		}
		av_free_packet(&packet);
	}
	return buffer;
}

void ffmpeg::clean() {
	av_free(buffer);
	av_free(pFrameRGB);
	av_free(pFrame);
	avcodec_close(pCodecCtx);
	avcodec_close(pCodecCtxOrig);
	avformat_close_input(&pFormatCtx);
}