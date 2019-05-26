#include <napi.h>
#include "screenshot.h"

namespace wrap {
    Napi::String hello(const Napi::CallbackInfo& info);
    Napi::Object init(Napi::Env env, Napi::Object exports);
    Napi::Value getBitmap(const Napi::CallbackInfo& info);
}