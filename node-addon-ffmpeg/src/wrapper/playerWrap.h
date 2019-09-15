#include <napi.h>
#include "../player/player.h"

namespace playerWrap {
    Napi::Value init(const Napi::CallbackInfo& info);
    Napi::Value readPacket(const Napi::CallbackInfo& info);
    Napi::Value decodeAudio(const Napi::CallbackInfo& info);
    Napi::Value decodeVideo(const Napi::CallbackInfo& info);
    Napi::Object getInfo(const Napi::CallbackInfo& info);
    Napi::Value updateAudioClock(const Napi::CallbackInfo& info);
    Napi::Value suspend(const Napi::CallbackInfo& info);
    Napi::Value resume(const Napi::CallbackInfo& info);
    Napi::Value update(const Napi::CallbackInfo& info);
    Napi::Value destroy(const Napi::CallbackInfo& info);

    Napi::Object initMethods(Napi::Env env, Napi::Object exports);

}