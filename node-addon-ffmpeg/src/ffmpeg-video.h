
#include <stdio.h>
#include <math.h>

extern "C" {
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libswscale/swscale.h>
}

#pragma warning(disable: 4996)

struct VideoParams
{
    double fps;
    int width;
    int height;
    const char* iformatName;
    const char* filename;
    int64_t duration;
    int64_t start;
    int64_t bitrate;
};

namespace ffmpeg {
    void SaveFrame(AVFrame* pFrame, int width, int height, int iFrame);
    uint8_t* extractRGB(AVFrame* frame, int width, int height);
    uint8_t* extractFrame();
    void getVideoInfo(AVFormatContext* pFormatCtx, AVCodecContext* pCodecCtx, int videoStream, VideoParams* videoParams);
    bool config(VideoParams* videoParams, const char* filename);
    void clean();
}


