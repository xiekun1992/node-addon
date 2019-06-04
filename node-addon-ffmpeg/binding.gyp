{
    "targets": [
        {
            "target_name": "ffmpeg",
            "cflags!": ["-fno-exceptions"],
            "cflags_cc!": ["-fno-exceptions"],
            "sources": [
                "src/main.cpp",
                "src/ffmpeg-video.cpp",
                "src/wrap.cpp"
            ],
            "include_dirs": [
                "<!@(node -p \"require('node-addon-api').include\")",
                "src/include/"
            ],
            "libraries": [
                "-Lsrc/lib/libavcodec.dll.a",
                "-Lsrc/lib/libavdevice.dll.a",
                "-Lsrc/lib/libavfilter.dll.a",
                "-Lsrc/lib/libavformat.dll.a",
                "-Lsrc/lib/libavutil.dll.a",
                "-Lsrc/lib/libpostproc.dll.a",
                "-Lsrc/lib/libswresample.dll.a",
                "-Lsrc/lib/libswscale.dll.a"
            ],
            "dependencies": [
                "<!(node -p \"require('node-addon-api').gyp\")"
            ],
            "defines": ["NAPI_DISABLE_CPP_EXCEPTIONS"]
        }
    ]
}