#include "wrap.h"

Napi::Value wrap::getBitmap(const Napi::CallbackInfo& info) {
    Napi::Env env = info.Env();
    BYTE* bytes = screenshot::getBitmap();
    Napi::Buffer<BYTE> buf = Napi::Buffer<BYTE>::New(env, bytes, 1600 * 900);
    // Napi::Value buf = info[0].As<Napi::Number>();
    free(bytes);
    return buf;
}
Napi::String wrap::hello(const Napi::CallbackInfo& info) {
    Napi::Env env = info.Env();
    return Napi::String::New(env, "hello world");
}
Napi::Object wrap::init(Napi::Env env, Napi::Object exports) {
    Napi::HandleScope scope(env);
    exports.Set("getBitmap", Napi::Function::New(env, wrap::getBitmap));
    exports.Set("hello", Napi::Function::New(env, wrap::hello));
    return exports;
}