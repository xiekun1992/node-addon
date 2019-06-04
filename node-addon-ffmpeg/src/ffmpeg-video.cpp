#include "ffmpeg-video.h"

void SaveFrame(AVFrame* pFrame, int width, int height, int iFrame) {
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

int extractFrame() {
	AVFormatContext* pFormatCtx = NULL;
	const char* filename = "D:\\Wildlife.wmv";
	AVCodecContext* pCodecCtxOrig = NULL;
	AVCodecContext* pCodecCtx = NULL;
	int videoStream = -1;
	AVCodec* pCodec = NULL;
	printf("00000\n");
	if (avformat_open_input(&pFormatCtx, filename, NULL, NULL) < 0) {
		return -1;
	}
	if (avformat_find_stream_info(pFormatCtx, NULL) < 0) {
		return -1;
	}
	for (unsigned int i = 0; i < pFormatCtx->nb_streams; i++) {
		if (pFormatCtx->streams[i]->codecpar->codec_type == AVMEDIA_TYPE_VIDEO) {
			videoStream = i;
			break;
		}
	}
	if (videoStream == -1) {
		return -1;
	}
	printf("11111\n");
	//pCodecCtx = pFormatCtx->streams[videoStream]->codec;
	pCodecCtx = avcodec_alloc_context3(NULL);
	if (avcodec_parameters_to_context(pCodecCtx, pFormatCtx->streams[videoStream]->codecpar) < 0) {
		return -1;
	}
	pCodec = avcodec_find_decoder(pCodecCtx->codec_id);
	if (!pCodec) {
		return -1;
	}
	printf("0011111\n");
	//if (avcodec_copy_context(pCodecCtx, pCodecCtxOrig) != 0) {
	//	return -1;
	//}
	if (avcodec_open2(pCodecCtx, pCodec, NULL) < 0) {
		return -1;
	}
	//system("PAUSE");
	//return 0;
	AVFrame* pFrame = NULL;
	pFrame = av_frame_alloc();
	if (!pFrame) {
		return -1;
	}
	AVFrame* pFrameRGB = NULL;
	pFrameRGB = av_frame_alloc();
	if (!pFrameRGB) {
		return -1;
	}
	printf("222222\n");
	uint8_t* buffer = NULL;
	int numBytes = avpicture_get_size(AV_PIX_FMT_RGB24, pCodecCtx->width, pCodecCtx->height);
	buffer = (uint8_t*)av_malloc(numBytes * sizeof(uint8_t));
	avpicture_fill((AVPicture*)pFrameRGB, buffer, AV_PIX_FMT_RGB24, pCodecCtx->width, pCodecCtx->height);
	struct SwsContext* sws_ctx = sws_getContext(pCodecCtx->width, pCodecCtx->height, 
		pCodecCtx->pix_fmt, pCodecCtx->width, pCodecCtx->height, AV_PIX_FMT_RGB24, SWS_BILINEAR, NULL, NULL, NULL);
	int frameFinished;
	AVPacket packet;
	int i = 0;
	printf("before while\n");
	while (av_read_frame(pFormatCtx, &packet) >= 0) {
		if (packet.stream_index == videoStream) {
			avcodec_decode_video2(pCodecCtx, pFrame, &frameFinished, &packet);
			if (frameFinished) {
				sws_scale(sws_ctx, (uint8_t const* const*)pFrame->data, pFrame->linesize, 0, pCodecCtx->height,
					pFrameRGB->data, pFrameRGB->linesize);
				if (++i <= 5) {
					printf("save frame\n");
					SaveFrame(pFrameRGB, pCodecCtx->width, pCodecCtx->height, i);
				}
			}
		}
		av_free_packet(&packet);
	}

	av_free(buffer);
	av_free(pFrameRGB);
	av_free(pFrame);
	avcodec_close(pCodecCtx);
	avcodec_close(pCodecCtxOrig);
	avformat_close_input(&pFormatCtx);

	// system("PAUSE");
	return 0;
}