#include <napi.h>

namespace wrap {
    Napi::Number extractFrame(const Napi::CallbackInfo& info);
    Napi::Object init(Napi::Env env, Napi::Object epxorts);
}