#include <stdio.h>

extern "C" {
#include <libavformat/avformat.h>
#include <libavcodec/avcodec.h>
#include <libswscale/swscale.h>
}
#pragma warning(disable: 4996)

AVFormatContext* fmtCtx = NULL;
AVCodecContext* codecCtx = NULL;
AVCodec* codec = NULL;
AVFrame* frameYUV = NULL;
AVFrame* frameRGB = NULL;
AVPacket* packet = NULL;
struct SwsContext* swsCtx = NULL;
const char* filename = "D:\\1.wmv";
int videoIndex = -1;
int numBytes;
int gotPicture;
int i = 0;
uint8_t* buffer = NULL;

void SaveFrame(AVFrame *pFrame, int width, int height, int iFrame) {
	FILE *pFile;
	char szFilename[32];
	int  y;

	// Open file
	sprintf(szFilename, "D:\\frame%d.ppm", iFrame);
	pFile = fopen(szFilename, "wb");
	if (pFile == NULL)
		return;

	// Write header
	fprintf(pFile, "P6\n%d %d\n255\n", width, height);

	// Write pixel data
	for (y = 0; y<height; y++)
		fwrite(pFrame->data[0] + y * pFrame->linesize[0], 1, width * 3, pFile);

	// Close file
	fclose(pFile);
}
void extractRGBFrame() {
	if (avformat_open_input(&fmtCtx, filename, NULL, NULL) < 0) {
		printf("avformat_open_input fail");
		return;
	}
	if (avformat_find_stream_info(fmtCtx, NULL) < 0) {
		printf("avformat_find_stream_info fail");
		return;
	}
	for (unsigned int i = 0; i < fmtCtx->nb_streams; i++) {
		if (fmtCtx->streams[i]->codecpar->codec_type == AVMEDIA_TYPE_VIDEO) {
			videoIndex = i;
			break;
		}
	}
	if (videoIndex < 0) {
		printf("videoIndex fail");
		return;
	}
	codecCtx = avcodec_alloc_context3(NULL);
	if (avcodec_parameters_to_context(codecCtx, fmtCtx->streams[videoIndex]->codecpar) < 0) {
		printf("avcodec_parameters_to_context fail");
		return;
	}
	codec = avcodec_find_decoder(codecCtx->codec_id);
	if (avcodec_open2(codecCtx, codec, NULL) < 0) {
		printf("avcodec_open2 fail");
		return;
	}
	packet = av_packet_alloc();
	av_init_packet(packet);
	frameYUV = av_frame_alloc();
	frameRGB = av_frame_alloc();

	numBytes = avpicture_get_size(AV_PIX_FMT_RGB24, codecCtx->width, codecCtx->height);
	buffer = (uint8_t*)av_malloc(numBytes * sizeof(uint8_t));
	swsCtx = sws_getContext(codecCtx->width, codecCtx->height, codecCtx->pix_fmt, codecCtx->width, codecCtx->height, AV_PIX_FMT_RGB24, SWS_BILINEAR, NULL, NULL, NULL);

	avpicture_fill((AVPicture*)frameRGB, buffer, AV_PIX_FMT_RGB24, codecCtx->width, codecCtx->height);

	while (av_read_frame(fmtCtx, packet) >= 0) {
		if (packet->stream_index == videoIndex) {
			avcodec_decode_video2(codecCtx, frameYUV, &gotPicture, packet);
			if (gotPicture) {
				// yuv to rgb
				sws_scale(swsCtx, (uint8_t const* const*)frameYUV->data, frameYUV->linesize, 0, codecCtx->height, frameRGB->data, frameRGB->linesize);
				if (i <= 5) {
					SaveFrame(frameRGB, codecCtx->width, codecCtx->height, i);
					i++;
				}
			}
		}
		av_free_packet(packet);
	}
}

int main() {
	extractRGBFrame();

	avformat_close_input(&fmtCtx);
	av_free(buffer);
	av_free(frameRGB);
	av_free(frameYUV);
	avcodec_close(codecCtx);

	system("PAUSE");
	return 0;
}