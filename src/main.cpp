#include <napi.h>
#include "wrap.h"

Napi::Object InitAll(Napi::Env env, Napi::Object exports) {
    
    return wrap::init(env, exports);
}

NODE_API_MODULE(NODE_GYP_MODULE_NAME, InitAll)