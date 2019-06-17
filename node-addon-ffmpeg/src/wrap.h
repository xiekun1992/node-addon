#include <napi.h>
#include "ffmpeg-video.h"


namespace wrap {
    Napi::Object config(const Napi::CallbackInfo& info);
    Napi::Value clean(const Napi::CallbackInfo& info);
    Napi::Value extractFramew(const Napi::CallbackInfo& info);
    Napi::Object init(Napi::Env env, Napi::Object epxorts);
}