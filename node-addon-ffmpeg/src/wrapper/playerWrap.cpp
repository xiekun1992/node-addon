#include "playerWrap.h"

Player player;
Napi::ThreadSafeFunction packetReadyTsfn, tsfn;
thread packetReadyThread;
thread nativeThread;
bool initialized = false, checkAudio = true, checkVideo = true;
int audioCheckInterval = 10;

class PacketWorker: public Napi::AsyncWorker {
public:
    PacketWorker(const Napi::Function& callback): Napi::AsyncWorker(callback){}
    ~PacketWorker(){}
    void Execute() {
        player.readPacket();
    }
    void OnOk() {
        printf("packet worker exit with ok\n");
    }
    void OnError() {
        printf("packet worker exit with error\n");
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
    if (!initialized) {
        Napi::Function func = info[0].As<Napi::Function>();
        PacketWorker* worker = new PacketWorker(func);
        worker->Queue();
    } else {
        player.readPacket();
    }
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
    VideoInfo map = player.getInfo();
    Napi::Object obj = Napi::Object::New(env);
    Napi::Object vobj = Napi::Object::New(env);
    Napi::Object aobj = Napi::Object::New(env);

    vobj.Set("fps", Napi::Number::New(env, map.video.fps));
    vobj.Set("width", Napi::Number::New(env, map.video.width));
    vobj.Set("height", Napi::Number::New(env, map.video.height));
    vobj.Set("iformatName", Napi::String::New(env, map.video.iformatName));
    vobj.Set("duration", Napi::Number::New(env, map.video.duration));
    vobj.Set("start", Napi::Number::New(env, map.video.start));
    vobj.Set("bitrate", Napi::Number::New(env, map.video.bitrate));

    aobj.Set("sampleRate", Napi::Number::New(env, map.audio.sampleRate));
    aobj.Set("channels", Napi::Number::New(env, map.audio.channels));
    aobj.Set("sampleFormat",Napi::String::New(env, map.audio.sampleFormat));

    obj.Set("video", vobj);
    obj.Set("audio", aobj);

    return obj;
}
Napi::Value playerWrap::updateAudioClock(const Napi::CallbackInfo& info) {
    int clock = info[0].As<Napi::Number>().Int32Value();
    player.updateAudioClock(clock);
    return info.Env().Undefined();
}

Napi::Value playerWrap::update(const Napi::CallbackInfo& info) {
    Napi::Env env = info.Env();
    if (!initialized) {
        initialized = true;
        printf("11\n");
        packetReadyTsfn = Napi::ThreadSafeFunction::New(env, info[0].As<Napi::Function>(), "ready", 0, 1, [](Napi::Env) {
            packetReadyThread.join();
        });
        packetReadyThread = thread([] {
            auto callback = [](Napi::Env env, Napi::Function jsCallback) {
                printf("audio callback call before\n");
                jsCallback.Call({});
                printf("audio callback call after\n");
            };
            while (true) {
                // printf("ready check %d\n", player.audioBufferSize);
                // printf("ready check %d\n", player.readyToPlay());
                if (checkAudio && player.readyToPlay() > 0) {
                    printf("ready check1\n");
                    // break;
                    napi_status status = packetReadyTsfn.BlockingCall(callback);
                    checkAudio = false;
                    // audioCheckInterval = 10 * 1000;
                    if (status != napi_ok) {
                        printf("audio loop call failed\n");
                        break;
                    }
                } else {
                    this_thread::sleep_for(chrono::milliseconds(audioCheckInterval));
                }
            }
            packetReadyTsfn.Release();
            //packetReadyThread.detach();
        });
        printf("11-end\n");
        tsfn = Napi::ThreadSafeFunction::New(env, info[1].As<Napi::Function>(), "Resource Name", 0, 1, [](Napi::Env) {
            nativeThread.join();
        });
        nativeThread = thread([] {
            auto callback = [](Napi::Env env, Napi::Function jsCallback) {
                if (player.decodeVideo() > 0) {
                    jsCallback.Call({Napi::Buffer<uint8_t>::New(env, player.buffer, player.videoBufferSize)});
                }
            };
            while (true) {
                if (checkVideo) {
                    napi_status status = tsfn.BlockingCall(callback);
                    if (status != napi_ok) {
                        printf("video loop call failed\n");
                        break;
                    }
                }
                this_thread::sleep_for(chrono::milliseconds(5));
            }
            tsfn.Release();
        });
    }
    printf("22-end\n");
    return Napi::Boolean::New(env, true);
}
Napi::Value playerWrap::destroy(const Napi::CallbackInfo& info) {
    
    // 调用c++重置解码上下文
    printf("OK-1\n");
    player.freeMedia();
    printf("OK\n");
    // 停止视频推送线程
    // packetReadyThread.detach();
    printf("release video thread safe function\n");

    return info.Env().Undefined();
}
Napi::Value playerWrap::suspend(const Napi::CallbackInfo& info) {
    checkVideo = false;
    checkAudio = false;
    // audioCheckInterval = 10 * 1000;
    player.suspendReadThread();
    return info.Env().Undefined();
}
Napi::Value playerWrap::resume(const Napi::CallbackInfo& info) {
    player.resumeReadThread();
    thread delay = thread([]{
        this_thread::sleep_for(chrono::milliseconds(100));
        // audioCheckInterval = 10;
        checkAudio = true;
        checkVideo = true;
    });
    delay.join();
    return info.Env().Undefined();
}
Napi::Value playerWrap::seek(const Napi::CallbackInfo& info) {
    if (info.Length() < 1) {
        Napi::Error::New(info.Env(), "expect 1 parameter").ThrowAsJavaScriptException();
        return Napi::Value();
    }
    // 暂停播放
    // suspend(info);

    int timestamp = info[0].As<Napi::Number>().Int32Value();
    player.seek(timestamp);
    // 继续播放
    // resume(info);
    return info.Env().Undefined();
}

Napi::Object playerWrap::initMethods(Napi::Env env, Napi::Object exports) {
    exports.Set("init", Napi::Function::New(env, playerWrap::init));
    exports.Set("readPacket", Napi::Function::New(env, playerWrap::readPacket));
    exports.Set("decodeAudio", Napi::Function::New(env, playerWrap::decodeAudio));
    exports.Set("decodeVideo", Napi::Function::New(env, playerWrap::decodeVideo));
    exports.Set("getInfo", Napi::Function::New(env, playerWrap::getInfo));
    exports.Set("updateAudioClock", Napi::Function::New(env, playerWrap::updateAudioClock));

    exports.Set("update", Napi::Function::New(env, playerWrap::update));
    exports.Set("destroy", Napi::Function::New(env, playerWrap::destroy));
    exports.Set("suspend", Napi::Function::New(env, playerWrap::suspend));
    exports.Set("resume", Napi::Function::New(env, playerWrap::resume));
    exports.Set("seek", Napi::Function::New(env, playerWrap::seek));
    return exports;
}