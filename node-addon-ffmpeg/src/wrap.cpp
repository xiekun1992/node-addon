#include "wrap.h"
#include "ffmpeg-video.h"

Napi::Value wrap::extractFramew(const Napi::CallbackInfo& info) {
    Napi::Env env = info.Env();
    uint8_t* bufptr = ffmpeg::extractFrame();
    Napi::Buffer<uint8_t> buf = Napi::Buffer<uint8_t>::New(env, bufptr, 1280 * 720 * 3);
    return buf;
}

Napi::Boolean wrap::config(const Napi::CallbackInfo& info) {
    Napi::Env env = info.Env();
    bool res = ffmpeg::config();
    return Napi::Boolean::New(env, res);
}
Napi::Value wrap::clean(const Napi::CallbackInfo& info) {
    Napi::Env env = info.Env();
    ffmpeg::clean();
    return Napi::Boolean::New(env, true);
    
}

Napi::Object wrap::init(Napi::Env env, Napi::Object exports) {
    exports.Set("ffmpeg", Napi::Function::New(env, wrap::extractFramew));
    exports.Set("config", Napi::Function::New(env, wrap::config));
    exports.Set("clean", Napi::Function::New(env, wrap::clean));
    return exports;
}