#include "ffmpeg-audio.h"
#include "utils/packet-queue.h"

#define MAX_AUDIO_FRAME_SIZE 192000 // 1 second of 48khz 32bit audio

AVFormatContext* fmtCtx = NULL;
AVCodecContext* codecCtx = NULL;
AVCodec* codec = NULL;
struct SwrContext *au_convert_ctx;
int audioIndex = -1;
int gotFrame = 0;
int oi = 0;
// const char* filename = "D:\\Wildlife.wmv";
int64_t in_channel_layout;
uint8_t	*out_buffer;

utils::PacketQueue queue;

bool audio::init(const char* filename) {

	if (avformat_open_input(&fmtCtx, filename, NULL, NULL) < 0) {
		return false;
	}
	if (avformat_find_stream_info(fmtCtx, NULL) < 0) {
		return false;
	}
	for (int i = 0; i < fmtCtx->nb_streams; i++) {
		if (audioIndex < 0 && fmtCtx->streams[i]->codecpar->codec_type == AVMEDIA_TYPE_AUDIO) {
			audioIndex = i;
			break;
		}
	}
	if (audioIndex < 0) {
		return false;
	}

	codecCtx = avcodec_alloc_context3(NULL);
	if (avcodec_parameters_to_context(codecCtx, fmtCtx->streams[audioIndex]->codecpar) < 0) {
		return false;
	}
	codec = avcodec_find_decoder(codecCtx->codec_id);
	if (avcodec_open2(codecCtx, codec, NULL) < 0) {
		return false;
	}

	AVFrame* frame;
	frame = av_frame_alloc();
	AVPacket* pkt;
	pkt = av_packet_alloc();

	uint64_t out_channel_layout = AV_CH_LAYOUT_STEREO;
	//nb_samples: AAC-1024 MP3-1152
	//int out_nb_samples = 44100;// codecCtx->frame_size;
	AVSampleFormat out_sample_fmt = AV_SAMPLE_FMT_S16;
	int out_sample_rate = codecCtx->sample_rate;
	//int out_channels = av_get_channel_layout_nb_channels(out_channel_layout);
	//Out Buffer Size
	//int out_buffer_size = av_samples_get_buffer_size(NULL, out_channels, out_nb_samples, out_sample_fmt, 1);

	out_buffer = (uint8_t *)av_malloc(MAX_AUDIO_FRAME_SIZE * 2);
	//pFrame = av_frame_alloc();

	//FIX:Some Codec's Context Information is missing
	in_channel_layout = av_get_default_channel_layout(codecCtx->channels);

	au_convert_ctx = swr_alloc();
	au_convert_ctx = swr_alloc_set_opts(au_convert_ctx, out_channel_layout, out_sample_fmt, out_sample_rate,
		in_channel_layout, codecCtx->sample_fmt, codecCtx->sample_rate, 0, NULL);
	swr_init(au_convert_ctx);

	utils::packetQueueInit(&queue);
	// int index = 0;
	while (av_read_frame(fmtCtx, pkt) >= 0) {
		if (pkt->stream_index == audioIndex) {
			printf("1 %d\n", pkt->size);
			utils::packetQueuePut(&queue, pkt);
			// avcodec_decode_audio4(codecCtx, frame, &gotFrame, pkt);
			// if (gotFrame) {
			// 	int size = av_samples_get_buffer_size(NULL, frame->channels, frame->nb_samples, codecCtx->sample_fmt, 1);
			// 	int out_buffer_size = size / 2;
			// 	//printf("%d %d %d\n", size, frame->sample_rate, frame->channels);
			// 	//SaveFrame(frame, size, 1, oi++);
			// 	// printf("index:%5d\t pts:%lld\t packet size:%d\n", index, pkt->pts, pkt->size);
			// 	swr_convert(au_convert_ctx, &out_buffer, MAX_AUDIO_FRAME_SIZE, (const uint8_t **)frame->data, frame->nb_samples);
			// 	//printf("out_buffer_size = %d, out_channels = %d, out_nb_samples = %d\n", out_buffer_size, out_channels, out_nb_samples);
			// 	// fwrite(out_buffer, 1, out_buffer_size, pFile);
			// 	index++;
			// 	audioParams->size = out_buffer_size;
			// 	audioParams->buffer = out_buffer;
			// 	return true;
			// }
		}
		// av_packet_unref(pkt);
	}
	return true;
}
bool audio::decode(AudioParams* audioParams) {
	AVPacket* pkt;
	pkt = av_packet_alloc();
	if (utils::packetQueueGet(&queue, pkt) < 0) {
		av_packet_unref(pkt);
		av_packet_free(&pkt);
		return false;
	}
	AVFrame* frame;
	frame = av_frame_alloc();
	gotFrame = 0;
	avcodec_decode_audio4(codecCtx, frame, &gotFrame, pkt);
	if (gotFrame) {
		int size = av_samples_get_buffer_size(NULL, frame->channels, frame->nb_samples, codecCtx->sample_fmt, 1);
		int out_buffer_size = size / 2;
		// interleaved 16bit pcm
		swr_convert(au_convert_ctx, &out_buffer, MAX_AUDIO_FRAME_SIZE, (const uint8_t **)frame->data, frame->nb_samples);
		
		audioParams->size = out_buffer_size;
		audioParams->buffer = out_buffer;
		// return true;
	}
	av_packet_unref(pkt);
	av_packet_free(&pkt);
	
	//FILE *pFile = fopen("D:\\output.pcm", "wb");
	// int index = 0;
	// while (av_read_frame(fmtCtx, pkt) >= 0) {
	// 	if (pkt->stream_index == audioIndex) {
	// 		avcodec_decode_audio4(codecCtx, frame, &gotFrame, pkt);
	// 		if (gotFrame) {
	// 			int size = av_samples_get_buffer_size(NULL, frame->channels, frame->nb_samples, codecCtx->sample_fmt, 1);
	// 			int out_buffer_size = size / 2;
	// 			//printf("%d %d %d\n", size, frame->sample_rate, frame->channels);
	// 			//SaveFrame(frame, size, 1, oi++);
	// 			// printf("index:%5d\t pts:%lld\t packet size:%d\n", index, pkt->pts, pkt->size);
	// 			swr_convert(au_convert_ctx, &out_buffer, MAX_AUDIO_FRAME_SIZE, (const uint8_t **)frame->data, frame->nb_samples);
	// 			//printf("out_buffer_size = %d, out_channels = %d, out_nb_samples = %d\n", out_buffer_size, out_channels, out_nb_samples);
	// 			// fwrite(out_buffer, 1, out_buffer_size, pFile);
	// 			index++;
	// 			audioParams->size = out_buffer_size;
	// 			audioParams->buffer = out_buffer;
	// 			return true;
	// 		}
	// 	}
	// 	av_packet_unref(pkt);
	// }
// 	swr_free(&au_convert_ctx);
// 	// fclose(pFile);

// 	system("pause");
// cleanup:
// 	avformat_close_input(&fmtCtx);
// 	avcodec_close(codecCtx);
// 	avcodec_free_context(&codecCtx);
// 	av_frame_free(&frame);
// 	av_packet_free(&pkt);
	return true;
}