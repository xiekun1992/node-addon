#include "player.h"

Player::Player() {}
Player::~Player() {}

void Player::init(const char* filename) {
	this->filename = filename;
	if (avformat_open_input(&audioFmtCtx, filename, NULL, NULL) < 0) {
		throw runtime_error(string("avformat_open_input failed"));
	}
	if (avformat_open_input(&videoFmtCtx, filename, NULL, NULL) < 0) {
		throw runtime_error(string("avformat_open_input failed"));
	}
	if (avformat_find_stream_info(audioFmtCtx, NULL) < 0) {
		throw runtime_error(string("avformat_find_stream_info failed"));
	}
	if (avformat_find_stream_info(videoFmtCtx, NULL) < 0) {
		throw runtime_error(string("avformat_find_stream_info failed"));
	}
	for (unsigned int i = 0; i < audioFmtCtx->nb_streams; i++) {
		if (audioStreamIndex < 0 && audioFmtCtx->streams[i]->codecpar->codec_type == AVMEDIA_TYPE_AUDIO) {
			audioStreamIndex = i;
		}
	}
	for (unsigned int i = 0; i < videoFmtCtx->nb_streams; i++) {
		if (videoStreamIndex < 0 && videoFmtCtx->streams[i]->codecpar->codec_type == AVMEDIA_TYPE_VIDEO) {
			videoStreamIndex = i;
		}
	}
	audioCodecCtx = avcodec_alloc_context3(NULL);
	if (avcodec_parameters_to_context(audioCodecCtx, audioFmtCtx->streams[audioStreamIndex]->codecpar) < 0) {
		throw runtime_error(string("audio avcodec_parameters_to_context failed"));
	}
	audioCodec = avcodec_find_decoder(audioCodecCtx->codec_id);
	if (avcodec_open2(audioCodecCtx, audioCodec, NULL) < 0) {
		throw runtime_error(string("audio avcodec_open2 failed"));
	}

	videoCodecCtx = avcodec_alloc_context3(NULL);
	if (avcodec_parameters_to_context(videoCodecCtx, videoFmtCtx->streams[videoStreamIndex]->codecpar) < 0) {
		throw runtime_error(string("video avcodec_parameters_to_context failed"));
	}
	videoCodec = avcodec_find_decoder(videoCodecCtx->codec_id);
	if (avcodec_open2(videoCodecCtx, videoCodec, NULL) < 0) {
		throw runtime_error(string("video avcodec_open2 failed"));
	}
	audioQueue.init();
	videoQueue.init();

	uint64_t out_channel_layout = AV_CH_LAYOUT_STEREO;
	//nb_samples: AAC-1024 MP3-1152
	//int out_nb_samples = 44100;// codecCtx->frame_size;
	AVSampleFormat out_sample_fmt = AV_SAMPLE_FMT_S16;
	int out_sample_rate = audioCodecCtx->sample_rate;
	//int out_channels = av_get_channel_layout_nb_channels(out_channel_layout);
	//Out Buffer Size
	//int out_buffer_size = av_samples_get_buffer_size(NULL, out_channels, out_nb_samples, out_sample_fmt, 1);

	uint8_t* out_buffer = (uint8_t*)av_malloc(MAX_AUDIO_FRAME_SIZE * 2);
	//pFrame = av_frame_alloc();

	//FIX:Some Codec's Context Information is missing
	int64_t in_channel_layout = av_get_default_channel_layout(audioCodecCtx->channels);

	audioConvertCtx = swr_alloc();
	audioConvertCtx = swr_alloc_set_opts(audioConvertCtx, out_channel_layout, out_sample_fmt, out_sample_rate,
		in_channel_layout, audioCodecCtx->sample_fmt, audioCodecCtx->sample_rate, 0, NULL);
	swr_init(audioConvertCtx);


}
map<string, string> Player::getInfo() {
	map<string, string> map;
	return map;
}
void Player::readAudioPacketThread() {
	AVPacket* pkt = NULL;
	pkt = av_packet_alloc();
	
	while (true) {
		if (audioQueue.size < 20 * 1024 * 1024) {
			if (av_read_frame(audioFmtCtx, pkt) >= 0) {
				//printf("packet stream_index=%d, audio index=%d, dts=%d\n", pkt->stream_index, audioStreamIndex, pkt->dts);
				if (pkt->stream_index == audioStreamIndex) {
					audioQueue.put(pkt);
				}
			}
			else {
				break;
			}
		}
		else {
			this_thread::sleep_for(chrono::milliseconds(10));
		}
	}
}
void Player::readVideoPacketThread() {
	AVPacket* pkt = NULL;
	pkt = av_packet_alloc();

	while (true) {
		if (videoQueue.size < 8 * 1024 * 1024) {
			if (av_read_frame(videoFmtCtx, pkt) >= 0) {
				if (pkt->stream_index == videoStreamIndex) {
					videoQueue.put(pkt);
				}
			}
			else {
				break;
			}
		}
		else {
			this_thread::sleep_for(chrono::milliseconds(10));
		}
	}
}
void Player::readPacket() {
	thread readAudio(&Player::readAudioPacketThread, this);
	thread readVideo(&Player::readVideoPacketThread, this);
	
	readAudio.join();
	readVideo.join();
}
void Player::decodeAudio() {
	//AVFrame* frame;
	//frame = av_frame_alloc();
	//AVPacket* pkt;
	//pkt = av_packet_alloc();
	//
	//uint64_t out_channel_layout = AV_CH_LAYOUT_STEREO;
	////nb_samples: AAC-1024 MP3-1152
	////int out_nb_samples = 44100;// codecCtx->frame_size;
	//AVSampleFormat out_sample_fmt = AV_SAMPLE_FMT_S16;
	//int out_sample_rate = audioCodecCtx->sample_rate;
	////int out_channels = av_get_channel_layout_nb_channels(out_channel_layout);
	////Out Buffer Size
	////int out_buffer_size = av_samples_get_buffer_size(NULL, out_channels, out_nb_samples, out_sample_fmt, 1);
	//
	//uint8_t* out_buffer = (uint8_t*)av_malloc(MAX_AUDIO_FRAME_SIZE * 2);
	////pFrame = av_frame_alloc();
	//
	////FIX:Some Codec's Context Information is missing
	//int64_t in_channel_layout = av_get_default_channel_layout(audioCodecCtx->channels);
	//
	//audioConvertCtx = swr_alloc();
	//audioConvertCtx = swr_alloc_set_opts(audioConvertCtx, out_channel_layout, out_sample_fmt, out_sample_rate,
	//	in_channel_layout, audioCodecCtx->sample_fmt, audioCodecCtx->sample_rate, 0, NULL);
	//swr_init(audioConvertCtx);

	AVPacket* pkt;
	pkt = av_packet_alloc();
	if (audioQueue.get(pkt) < 0) {
		av_packet_free(&pkt);
		return;
	}
	AVFrame* frame;
	frame = av_frame_alloc();
	int gotFrame = 0;
	avcodec_decode_audio4(audioCodecCtx, frame, &gotFrame, pkt);
	if (gotFrame) {
		int size = av_samples_get_buffer_size(NULL, frame->channels, frame->nb_samples, audioCodecCtx->sample_fmt, 1);
		int out_buffer_size = size / 2;
		// interleaved 16bit pcm
		swr_convert(audioConvertCtx, &audioBuffer, MAX_AUDIO_FRAME_SIZE, (const uint8_t * *)frame->data, frame->nb_samples);

		//audioParams->size = out_buffer_size;
		//audioParams->buffer = out_buffer;
		// return true;
	}
	av_packet_free(&pkt);
}
void Player::decodeVideo() {
	//videoFrame = av_frame_alloc();
	//if (!videoFrame) {
	//	throw runtime_error(string("video av_frame_alloc failed"));
	//}
	//videoFrameRGB = av_frame_alloc();
	//if (!videoFrameRGB) {
	//	throw runtime_error(string("video av_frame_alloc failed"));
	//}
	//
	//int numBytes = avpicture_get_size(AV_PIX_FMT_RGB24, videoCodecCtx->width, videoCodecCtx->height);
	//buffer = (uint8_t*)av_malloc(numBytes * sizeof(uint8_t));
	//
	//avpicture_fill((AVPicture*)videoFrameRGB, buffer, AV_PIX_FMT_RGB24, videoCodecCtx->width, videoCodecCtx->height);
	//swsCtx = sws_getContext(
	//	videoCodecCtx->width, videoCodecCtx->height, videoCodecCtx->pix_fmt, videoCodecCtx->width, videoCodecCtx->height,
	//	AV_PIX_FMT_RGB24, SWS_BILINEAR, NULL, NULL, NULL);
}
