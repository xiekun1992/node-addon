#include <stdio.h>

extern "C" {
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libswscale/swscale.h>
}

#pragma warning(disable: 4996)

namespace ffmpeg {
    void SaveFrame(AVFrame* pFrame, int width, int height, int iFrame);
    int extractFrame();
}