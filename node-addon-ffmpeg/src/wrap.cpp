#include "wrap.h"
#include "ffmpeg-video.h"

Napi::Number wrap::extractFrame(const Napi::CallbackInfo& info) {
    Napi::Env env = info.Env();
    int n = ffmpeg::extractFrame();
    Napi::Number num = (Napi::Number::New(env, (double)n));
    return num;
}

Napi::Object wrap::init(Napi::Env env, Napi::Object exports) {
    exports.Set("ffmpeg", Napi::Function::New(env, wrap::extractFrame));
    return exports;
}