#include "wrap.h"

struct VideoParams params;
struct AudioParams aParams;

Napi::Value wrap::extractFramew(const Napi::CallbackInfo& info) {
    Napi::Env env = info.Env();
    uint8_t* bufptr = ffmpeg::extractFrame();
    Napi::Buffer<uint8_t> buf = Napi::Buffer<uint8_t>::New(env, bufptr, params.width * params.height * 3);
    return buf;
}

Napi::Object wrap::config(const Napi::CallbackInfo& info) {
    Napi::Env env = info.Env();
    std::string filename = info[0].As<Napi::String>().ToString();
    bool res = ffmpeg::config(&params, filename.c_str());
    Napi::Object obj = Napi::Object::New(env);
    if (res) {
        obj.Set("fps", params.fps);
        obj.Set("width", params.width);
        obj.Set("height", params.height);
        obj.Set("iformatName", params.iformatName);
        obj.Set("filename", params.filename);
        obj.Set("duration", params.duration);
        obj.Set("start", params.start);
        obj.Set("bitrate", params.bitrate);
    }
    return obj;
}
Napi::Value wrap::clean(const Napi::CallbackInfo& info) {
    Napi::Env env = info.Env();
    ffmpeg::clean();
    return Napi::Boolean::New(env, true);
    
}

Napi::Boolean wrap::initAudio(const Napi::CallbackInfo& info) {
    Napi::Env env = info.Env();
    bool res = audio::init();
    Napi::Boolean b = Napi::Boolean::New(env, res);
    return b;
}
Napi::Value wrap::decodeAudio(const Napi::CallbackInfo& info) {
    Napi::Env env = info.Env();
    bool res = audio::decode(&aParams);
    if (res) {
        return Napi::Buffer<uint8_t>::New(env, aParams.buffer, aParams.size);
    }
    return env.Null();
}


Napi::Object wrap::init(Napi::Env env, Napi::Object exports) {
    exports.Set("extractRGBFrame", Napi::Function::New(env, wrap::extractFramew));
    exports.Set("config", Napi::Function::New(env, wrap::config));
    exports.Set("clean", Napi::Function::New(env, wrap::clean));

    exports.Set("initAudio", Napi::Function::New(env, wrap::initAudio));
    exports.Set("decodeAudio", Napi::Function::New(env, wrap::decodeAudio));
    return exports;
}