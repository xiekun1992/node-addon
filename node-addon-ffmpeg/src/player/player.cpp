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
	if (audioStreamIndex > -1) {
		audioCodecCtx = avcodec_alloc_context3(NULL);
		if (avcodec_parameters_to_context(audioCodecCtx, audioFmtCtx->streams[audioStreamIndex]->codecpar) < 0) {
			throw runtime_error(string("audio avcodec_parameters_to_context failed"));
		}
		audioCodec = avcodec_find_decoder(audioCodecCtx->codec_id);
		if (avcodec_open2(audioCodecCtx, audioCodec, NULL) < 0) {
			throw runtime_error(string("audio avcodec_open2 failed"));
		}
		audioQueue.init(MAX_AUDIO_FRAME_SIZE * 2, 160); // 第二个参数小了会导致声音有卡住的问题

		// 音频转换设置
		uint64_t out_channel_layout = AV_CH_LAYOUT_STEREO;
		//nb_samples: AAC-1024 MP3-1152
		//int out_nb_samples = 44100;// codecCtx->frame_size;
		AVSampleFormat out_sample_fmt = AV_SAMPLE_FMT_S16;
		int out_sample_rate = audioCodecCtx->sample_rate;
		//int out_channels = av_get_channel_layout_nb_channels(out_channel_layout);
		//Out Buffer Size
		//int out_buffer_size = av_samples_get_buffer_size(NULL, out_channels, out_nb_samples, out_sample_fmt, 1);

		//audioBuffer = (uint8_t*)av_malloc(MAX_AUDIO_FRAME_SIZE * 2);
		//pFrame = av_frame_alloc();

		//FIX:Some Codec's Context Information is missing
		int64_t in_channel_layout = av_get_default_channel_layout(audioCodecCtx->channels);

		audioConvertCtx = swr_alloc();
		// 保持输出采样率，改变了采样格式（交错式16位有符号）和通道数（2）
		audioConvertCtx = swr_alloc_set_opts(audioConvertCtx, out_channel_layout, out_sample_fmt, out_sample_rate,
			in_channel_layout, audioCodecCtx->sample_fmt, audioCodecCtx->sample_rate, 0, NULL);
		swr_init(audioConvertCtx);
	}

	for (unsigned int i = 0; i < videoFmtCtx->nb_streams; i++) {
		if (videoStreamIndex < 0 && videoFmtCtx->streams[i]->codecpar->codec_type == AVMEDIA_TYPE_VIDEO) {
			videoStreamIndex = i;
		}
	}
	if (videoStreamIndex > -1) {
		videoCodecCtx = avcodec_alloc_context3(NULL);
		if (avcodec_parameters_to_context(videoCodecCtx, videoFmtCtx->streams[videoStreamIndex]->codecpar) < 0) {
			throw runtime_error(string("video avcodec_parameters_to_context failed"));
		}
		videoCodec = avcodec_find_decoder(videoCodecCtx->codec_id);
		if (avcodec_open2(videoCodecCtx, videoCodec, NULL) < 0) {
			throw runtime_error(string("video avcodec_open2 failed"));
		}
		// 视频转换设置

		//videoFrameRGB = av_frame_alloc();
		//if (!videoFrameRGB) {
		//	throw runtime_error(string("video av_frame_alloc failed"));
		//}
		int numBytes = av_image_get_buffer_size(AV_PIX_FMT_RGB24, videoCodecCtx->width, videoCodecCtx->height, 1);
		videoBufferSize = numBytes * sizeof(uint8_t);
		//buffer = (uint8_t*)av_malloc(videoBufferSize);

		//avpicture_fill((AVPicture*)videoFrameRGB, buffer, AV_PIX_FMT_RGB24, videoCodecCtx->width, videoCodecCtx->height);
		//swsCtx = sws_getContext(
		//	videoCodecCtx->width, videoCodecCtx->height, videoCodecCtx->pix_fmt, videoCodecCtx->width, videoCodecCtx->height,
		//	AV_PIX_FMT_RGB24, SWS_BILINEAR, NULL, NULL, NULL);

		videoQueue.init(videoBufferSize, 20);
	}
}
map<string, map<string, string>> Player::getInfo() {
	map<string, map<string, string>> wrap;
	if (videoStreamIndex > -1) {
		map<string, string> vmap;
		vmap["fps"] = string(to_string(round(av_q2d(videoFmtCtx->streams[videoStreamIndex]->avg_frame_rate) * 100) / 100));
		vmap["width"] = string(to_string(videoCodecCtx->width));
		vmap["height"] = string(to_string(videoCodecCtx->height));
		vmap["iformatName"] = string(videoFmtCtx->iformat->name);
		int64_t duration = videoFmtCtx->duration + (videoFmtCtx->duration <= INT64_MAX - 5000 ? 5000 : 0);
		vmap["duration"] = string(to_string(duration / 1000));

		if (videoFmtCtx->start_time != AV_NOPTS_VALUE) {
			vmap["start"] = string(to_string(videoFmtCtx->start_time));
		}
		if (videoFmtCtx->bit_rate) {
			vmap["bitrate"] = string(to_string(videoFmtCtx->bit_rate / 1000));
		}

		wrap["video"] = vmap;
	}
	if (audioStreamIndex > -1) {
		map<string, string> amap;
		amap["sampleRate"] = string(to_string(audioCodecCtx->sample_rate));
		amap["channels"] = string(to_string(audioCodecCtx->channels));
		amap["sampleFormat"] = string(to_string(audioCodecCtx->sample_fmt));

		wrap["audio"] = amap;
	}
	return wrap;
}
void Player::readAudioPacketThread() {
	if (audioStreamIndex < 0) {
		return;
	}
	while (true) {
		if (audioQueue.avail() > 0) {
			AVPacket* pkt = av_packet_alloc();
			if (av_read_frame(audioFmtCtx, pkt) >= 0) {
				//printf("packet stream_index=%d, audio index=%d, dts=%d\n", pkt->stream_index, audioStreamIndex, pkt->dts);
				if (pkt->stream_index == audioStreamIndex) {
					if (avcodec_send_packet(audioCodecCtx, pkt) >= 0) {
						AVFrame* frame = av_frame_alloc();
						avcodec_receive_frame(audioCodecCtx, frame);
						//audioBuffer = (uint8_t*)av_malloc(MAX_AUDIO_FRAME_SIZE * 2);
						int size = av_samples_get_buffer_size(NULL, frame->channels, frame->nb_samples, audioCodecCtx->sample_fmt, 1) / 2;
						audioQueue.getReallocEmptyFrame(&audioBuffer, size);
						// interleaved 16bit pcm
						swr_convert(audioConvertCtx, &audioBuffer, MAX_AUDIO_FRAME_SIZE, (const uint8_t * *)frame->data, frame->nb_samples);
						av_frame_free(&frame);
					}
					//audioQueue.put(pkt);
				}
			}
			else {
				//break;
				this_thread::sleep_for(chrono::milliseconds(10));
			}
			av_packet_free(&pkt);
		}
		else {
			//break;
			this_thread::sleep_for(chrono::milliseconds(10));
		}
	}
}
void Player::readVideoPacketThread() {
	if (videoStreamIndex < 0) {
		return;
	}
	while (true) {
		if (videoQueue.avail() > 0) {
			AVPacket* pkt = av_packet_alloc();
			if (av_read_frame(videoFmtCtx, pkt) >= 0) {
				if (pkt->stream_index == videoStreamIndex) {
					AVFrame* videoFrame = av_frame_alloc();
					AVFrame* videoFrameRGB = av_frame_alloc();
					if (avcodec_send_packet(videoCodecCtx, pkt) >= 0) {
						avcodec_receive_frame(videoCodecCtx, videoFrame);
						videoQueue.getEmptyFrame(&buffer, 0);
						av_image_fill_arrays(videoFrameRGB->data, videoFrameRGB->linesize, buffer, AV_PIX_FMT_RGB24, videoCodecCtx->width, videoCodecCtx->height, 1);
						struct SwsContext* swsCtx = sws_getContext(videoCodecCtx->width, videoCodecCtx->height, videoCodecCtx->pix_fmt, videoCodecCtx->width, videoCodecCtx->height, AV_PIX_FMT_RGB24, SWS_BILINEAR, NULL, NULL, NULL);
						sws_scale(swsCtx, (uint8_t const* const*)videoFrame->data, videoFrame->linesize, 0, videoCodecCtx->height, videoFrameRGB->data, videoFrameRGB->linesize);
						sws_freeContext(swsCtx);
					}
					av_frame_free(&videoFrameRGB);
					av_frame_free(&videoFrame);
				}
			}
			else {
				//break;
				this_thread::sleep_for(chrono::milliseconds(10));
			}
			// 使用av_free_packet会有内存泄漏大约一分钟5M
			av_packet_free(&pkt);
		}
		else {
			//break;
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
void Player::updateAudioClock(int timeDelta) {
	audioClock = timeDelta;
	//readPacket();
}
int Player::decodeAudio() {
	if (audioStreamIndex < 0) {
		return -1;
	}
	//AVPacket* pkt = av_packet_alloc();
	//if (audioQueue.get(pkt) < 0) {
	//	av_packet_free(&pkt);
	//	return;
	//}
	//if (avcodec_send_packet(audioCodecCtx, pkt) >= 0) {
	//	AVFrame* frame = av_frame_alloc();
	//	avcodec_receive_frame(audioCodecCtx, frame);
	//	audioBuffer = (uint8_t*)av_malloc(MAX_AUDIO_FRAME_SIZE * 2);
	//	audioBufferSize = av_samples_get_buffer_size(NULL, frame->channels, frame->nb_samples, audioCodecCtx->sample_fmt, 1) / 2;
	//	// interleaved 16bit pcm
	//	swr_convert(audioConvertCtx, &audioBuffer, MAX_AUDIO_FRAME_SIZE, (const uint8_t * *)frame->data, frame->nb_samples);
	//	av_frame_free(&frame);
	//}
	////av_packet_unref(pkt);
	//av_packet_free(&pkt);
	return audioQueue.getDecodedFrame(&audioBuffer, &audioBufferSize);
}
int Player::decodeVideo() {
	if (videoStreamIndex < 0) {
		return -1;
	}
	int tmp = 0;
	return videoQueue.getDecodedFrame(&buffer, &tmp);
}
