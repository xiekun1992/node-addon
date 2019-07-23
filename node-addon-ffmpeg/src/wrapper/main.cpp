#include <napi.h>
#include "playerWrap.h"

Napi::Object InitAll(Napi::Env env, Napi::Object exports) {
    return playerWrap::initMethods(env, exports);
}

NODE_API_MODULE(NODE_GYP_MODULE_NAME, InitAll);