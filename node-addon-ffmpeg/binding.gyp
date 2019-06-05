{
    "variables": {
        "root%": "./src/"
    },
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
                "d:/a_gitlab/node-addons/node-addon-ffmpeg/src/include"
            ],
            "libraries": [
                "-ld:/a_gitlab/node-addons/node-addon-ffmpeg/src/lib/libavcodec.dll.a",
                "-ld:/a_gitlab/node-addons/node-addon-ffmpeg/src/lib/libavdevice.dll.a",
                "-ld:/a_gitlab/node-addons/node-addon-ffmpeg/src/lib/libavfilter.dll.a",
                "-ld:/a_gitlab/node-addons/node-addon-ffmpeg/src/lib/libavformat.dll.a",
                "-ld:/a_gitlab/node-addons/node-addon-ffmpeg/src/lib/libavutil.dll.a",
                "-ld:/a_gitlab/node-addons/node-addon-ffmpeg/src/lib/libpostproc.dll.a",
                "-ld:/a_gitlab/node-addons/node-addon-ffmpeg/src/lib/libswresample.dll.a",
                "-ld:/a_gitlab/node-addons/node-addon-ffmpeg/src/lib/libswscale.dll.a",
            ],
            # "ldflags": [
            #     "-L<@(root)/lib",
            #     "-Wl,-rpath,<@(root)/lib/libavcodec.dll.a",
            #     "-Wl,-rpath,<@(root)/lib/libavdevice.dll.a",
            #     "-Wl,-rpath,<@(root)/lib/libavfilter.dll.a",
            #     "-Wl,-rpath,<@(root)/lib/libavformat.dll.a",
            #     "-Wl,-rpath,<@(root)/lib/libavutil.dll.a",
            #     "-Wl,-rpath,<@(root)/lib/libpostproc.dll.a",
            #     "-Wl,-rpath,<@(root)/lib/libswresample.dll.a",
            #     "-Wl,-rpath,<@(root)/lib/libswscale.dll.a",
            # ],
            "link_settings": {
                # "libraries": ["-ldl"]
            },
            'msvs_settings': {
              'VCLinkerTool': {
                'AdditionalLibraryDirectories': [
                  'd:/a_gitlab/node-addons/node-addon-ffmpeg/src/lib'
                ],
              },
            },
            "dependencies": [
                "<!(node -p \"require('node-addon-api').gyp\")",
                # "src/lib/avcodec.lib",
                # "src/lib/avdevice.lib",
                # "src/lib/avfilter.lib",
                # "src/lib/avformat.lib",
                # "src/lib/avutil.lib",
                # "src/lib/postproc.lib",
                # "src/lib/swresample.lib",
                # "src/lib/swscale.lib",
            ],
            "defines": ["NAPI_DISABLE_CPP_EXCEPTIONS"]
        }
    ]
}