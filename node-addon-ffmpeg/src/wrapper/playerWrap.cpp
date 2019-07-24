#include "playerWrap.h"

Player player;

Napi::Value playerWrap::init(const Napi::CallbackInfo& info) {
    Napi::Env env = info.Env();
    std::string filename = info[0].As<Napi::String>().ToString();
    player.init(filename.c_str());
    return env.Undefined();
}
Napi::Value playerWrap::readPacket(const Napi::CallbackInfo& info) {
    Napi::Env env = info.Env();
    player.readPacket();
    return env.Undefined();
}
Napi::Value playerWrap::decodeAudio(const Napi::CallbackInfo& info) {
    Napi::Env env = info.Env();
    player.decodeAudio();
    return Napi::Buffer<uint8_t>::New(env, player.audioBuffer, player.audioBufferSize);
}
Napi::Value playerWrap::decodeVideo(const Napi::CallbackInfo& info) {
    Napi::Env env = info.Env();
    return env.Undefined();
}
Napi::Object playerWrap::getInfo(const Napi::CallbackInfo& info) {
    Napi::Env env = info.Env();
    map<string, map<string, string>> map = player.getInfo();
    Napi::Object obj = Napi::Object::New(env);
    Napi::Object vobj = Napi::Object::New(env);
    Napi::Object aobj = Napi::Object::New(env);

    vobj.Set("fps", map["video"]["fps"]);
    vobj.Set("width", map["video"]["width"]);
    vobj.Set("height", map["video"]["height"]);
    vobj.Set("iformatName", map["video"]["iformatName"]);
    vobj.Set("duration", map["video"]["duration"]);
    vobj.Set("start", map["video"]["start"]);
    vobj.Set("bitrate", map["video"]["bitrate"]);

    aobj.Set("sampleRate", map["audio"]["sampleRate"]);
    aobj.Set("channels", map["audio"]["channels"]);
    aobj.Set("sampleFormat", map["audio"]["sampleFormat"]);

    obj.Set("video", vobj);
    obj.Set("audio", aobj);

    return obj;
}

Napi::Object playerWrap::initMethods(Napi::Env env, Napi::Object exports) {
    exports.Set("init", Napi::Function::New(env, playerWrap::init));
    exports.Set("readPacket", Napi::Function::New(env, playerWrap::readPacket));
    exports.Set("decodeAudio", Napi::Function::New(env, playerWrap::decodeAudio));
    exports.Set("decodeVideo", Napi::Function::New(env, playerWrap::decodeVideo));
    exports.Set("getInfo", Napi::Function::New(env, playerWrap::getInfo));
    return exports;
}