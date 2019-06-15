#include <napi.h>

namespace wrap {
    Napi::Boolean config(const Napi::CallbackInfo& info);
    Napi::Value clean(const Napi::CallbackInfo& info);
    Napi::Value extractFramew(const Napi::CallbackInfo& info);
    Napi::Object init(Napi::Env env, Napi::Object epxorts);
}