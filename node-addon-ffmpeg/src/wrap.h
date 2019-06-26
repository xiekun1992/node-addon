#include <napi.h>
#include "ffmpeg-video.h"
#include "ffmpeg-audio.h"


namespace wrap {
    Napi::Object config(const Napi::CallbackInfo& info);
    Napi::Value clean(const Napi::CallbackInfo& info);
    Napi::Value extractFramew(const Napi::CallbackInfo& info);

    Napi::Boolean initAudio(const Napi::CallbackInfo& info);
    Napi::Value decodeAudio(const Napi::CallbackInfo& info);

    Napi::Object init(Napi::Env env, Napi::Object epxorts);
}