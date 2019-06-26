{
    "targets": [
        {
            "target_name": "ffmpeg",
            "cflags!": ["-fno-exceptions"],
            "cflags_cc!": ["-fno-exceptions"],
            "sources": [
                "src/utils/packet-queue.cpp",
                "src/main.cpp",
                "src/ffmpeg-video.cpp",
                "src/ffmpeg-audio.cpp",
                "src/wrap.cpp"
            ],
            "include_dirs": [
                "<!@(node -p \"require('node-addon-api').include\")",
                "<(module_root_dir)/src/include/"
            ],
            "copies": [
                {
                    "destination": "<(module_root_dir)/build/Release/",
                    "files": [ 
                        "<(module_root_dir)/src/bin/avcodec-58.dll",
                        "<(module_root_dir)/src/bin/avdevice-58.dll",
                        "<(module_root_dir)/src/bin/avfilter-7.dll",
                        "<(module_root_dir)/src/bin/avformat-58.dll",
                        "<(module_root_dir)/src/bin/avutil-56.dll",
                        "<(module_root_dir)/src/bin/postproc-55.dll",
                        "<(module_root_dir)/src/bin/swresample-3.dll",
                        "<(module_root_dir)/src/bin/swscale-5.dll",
                    ]
                }
            ],
            "link_settings": {
                "libraries": [
                    "<(module_root_dir)/src/lib/avcodec.lib",
                    "<(module_root_dir)/src/lib/avdevice.lib",
                    "<(module_root_dir)/src/lib/avfilter.lib",
                    "<(module_root_dir)/src/lib/avformat.lib",
                    "<(module_root_dir)/src/lib/avutil.lib",
                    "<(module_root_dir)/src/lib/postproc.lib",
                    "<(module_root_dir)/src/lib/swresample.lib",
                    "<(module_root_dir)/src/lib/swscale.lib",
                ],
            },
            "msvs_settings": {
              'VCLinkerTool': {
                'AdditionalLibraryDirectories': [
                  '<(module_root_dir)/src/lib/'
                ],
              },
            },
            "dependencies": [
                "<!(node -p \"require('node-addon-api').gyp\")",
            ],
            "defines": ["NAPI_DISABLE_CPP_EXCEPTIONS"]
        }
    ]
}