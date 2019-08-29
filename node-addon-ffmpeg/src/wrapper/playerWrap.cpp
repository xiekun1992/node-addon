#include "playerWrap.h"

Player player;


class PacketWorker: public Napi::AsyncWorker {
public:
    PacketWorker(const Napi::Function& callback): Napi::AsyncWorker(callback){}
    ~PacketWorker(){}
    void Execute() {
        player.readPacket();
    }
};

Napi::Value playerWrap::init(const Napi::CallbackInfo& info) {
    Napi::Env env = info.Env();
    std::string filename = info[0].As<Napi::String>().ToString();
    player.init(filename.c_str());
    return env.Undefined();
}
Napi::Value playerWrap::readPacket(const Napi::CallbackInfo& info) {
    Napi::Env env = info.Env();
    Napi::Function func = info[0].As<Napi::Function>();
    PacketWorker* worker = new PacketWorker(func);
    worker->Queue();
    return env.Undefined();
}
Napi::Value playerWrap::decodeAudio(const Napi::CallbackInfo& info) {
    Napi::Env env = info.Env();
    if (player.decodeAudio() < 0) {
        return env.Null();
    }
    return Napi::Buffer<uint8_t>::New(env, player.audioBuffer, player.audioBufferSize);
}

Napi::Value playerWrap::decodeVideo(const Napi::CallbackInfo& info) {
    Napi::Env env = info.Env();
    // return env.Undefined();
    if (player.decodeVideo() < 0) {
        return env.Null();
    }
    return Napi::Buffer<uint8_t>::New(env, player.buffer, player.videoBufferSize);
    // Napi::Buffer<uint8_t> buffer = Napi::Buffer<uint8_t>::New(env, player.buffer, player.videoBufferSize,
    //     [](Napi::Env env, uint8_t* finalizeData) {
    //         finalizeData = NULL;
    //         // delete[] finalizeData;
    //     });
    // return buffer;
}
Napi::Object playerWrap::getInfo(const Napi::CallbackInfo& info) {
    Napi::Env env = info.Env();
    map<string, map<string, string>> map = player.getInfo();
    Napi::Object obj = Napi::Object::New(env);
    Napi::Object vobj = Napi::Object::New(env);
    Napi::Object aobj = Napi::Object::New(env);

    vobj.Set("fps", map["video"]["fps"]);
    vobj.Set("width", map["video"]["width"]);
    vobj.Set("height", map["video"]["height"]);
    vobj.Set("iformatName", map["video"]["iformatName"]);
    vobj.Set("duration", map["video"]["duration"]);
    vobj.Set("start", map["video"]["start"]);
    vobj.Set("bitrate", map["video"]["bitrate"]);

    aobj.Set("sampleRate", map["audio"]["sampleRate"]);
    aobj.Set("channels", map["audio"]["channels"]);
    aobj.Set("sampleFormat", map["audio"]["sampleFormat"]);

    obj.Set("video", vobj);
    obj.Set("audio", aobj);

    return obj;
}

Napi::ThreadSafeFunction tsfn;
std::thread nativeThread;

Napi::Value update(const Napi::CallbackInfo& info) {
    Napi::Env env = info.Env();
    tsfn = Napi::ThreadSafeFunction::New(env, info[0].As<Napi::Function>(), "Resource Name", 0, 1, [](Napi::Env) {
        nativeThread.join();
    });
    nativeThread = thread([] {
        auto callback = [](Napi::Env env, Napi::Function jsCallback) {
            player.decodeVideo();
            player.decodeAudio();
            jsCallback.Call({
                Napi::Buffer<uint8_t>::New(env, player.buffer, player.videoBufferSize),
                Napi::Buffer<uint8_t>::New(env, player.audioBuffer, player.audioBufferSize)
            });
        };
        while(true) {
            napi_status status = tsfn.BlockingCall(callback);
            if (status != napi_ok) {
                printf("call failed\n");
                break;
            }
            this_thread::sleep_for(chrono::milliseconds(10));
        }
        tsfn.Release();
    });
    return Napi::Boolean::New(env, true);
}

Napi::Object playerWrap::initMethods(Napi::Env env, Napi::Object exports) {
    exports.Set("init", Napi::Function::New(env, playerWrap::init));
    exports.Set("readPacket", Napi::Function::New(env, playerWrap::readPacket));
    exports.Set("decodeAudio", Napi::Function::New(env, playerWrap::decodeAudio));
    exports.Set("decodeVideo", Napi::Function::New(env, playerWrap::decodeVideo));
    exports.Set("getInfo", Napi::Function::New(env, playerWrap::getInfo));

    exports.Set("update", Napi::Function::New(env, update));
    return exports;
}