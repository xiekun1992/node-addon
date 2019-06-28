#include <stdio.h> 
extern "C" {
    #include <libavformat/avformat.h>
    #include <libavcodec/avcodec.h>
    #include <libswresample/swresample.h>
}

#pragma warning(disable: 4996)

struct AudioParams {
    uint8_t* buffer;
    int size;
};

namespace audio {
    bool init(const char* filename);
    bool decode(AudioParams* audioParams);
}