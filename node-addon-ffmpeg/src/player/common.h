#pragma once

#include <iostream>
#include <map>
#include <thread>
#include <chrono>
#include <math.h>
#include <string>

extern "C" {
#include <libavformat/avformat.h>
#include <libavcodec/avcodec.h>
#include <libswscale/swscale.h>
#include <libswresample/swresample.h>
#include <libavutil/imgutils.h>
}

//#pragma warning(disable: 4996)

using namespace std;