#include <napi.h>
#include "../player/player.h"

namespace playerWrap {
	// p.init("D:\\Wildlife.wmv");
	// p.readPacket();
	// p.decodeAudio();
    Napi::Value init(const Napi::CallbackInfo& info);
    Napi::Value readPacket(const Napi::CallbackInfo& info);
    Napi::Value decodeAudio(const Napi::CallbackInfo& info);
    Napi::Value decodeVideo(const Napi::CallbackInfo& info);
    Napi::Object getInfo(const Napi::CallbackInfo& info);
    Napi::Value updateAudioClock(const Napi::CallbackInfo& info);

    Napi::Object initMethods(Napi::Env env, Napi::Object exports);

}