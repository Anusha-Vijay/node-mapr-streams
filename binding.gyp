{
  "targets": [
    {
      "target_name": "mstreams",
      "sources": [
                "./src/addon.c"
                "./src/mstreams.h",
                "./src/mstreams.cc",
                "./src/mproducer.h",
                "./src/mproducer.cc",
                "./src/mconsumer.h",
                "./src/mconsumer.cc"
                ],
      "include_dirs": [
            "<!@(node -p \"require('node-addon-api').include\")",
            "/opt/mapr/include/librdkafka",
      ],
      "dependencies": [
            "<!(node -p \"require('node-addon-api').gyp\")"
      ],
      "library_dirs": [
            "/opt/mapr/include/librdkafka",
      ],
      "libraries": [
            "/opt/mapr/lib/librdkafka.so"
      ],
      'cflags!': [ '-fno-exceptions' ],
        'cflags_cc!': [ '-fno-exceptions' ],
        'xcode_settings': {
          'GCC_ENABLE_CPP_EXCEPTIONS': 'YES',
          'CLANG_CXX_LIBRARY': 'libc++',

          'MACOSX_DEPLOYMENT_TARGET': '10.7',
        },
        'msvs_settings': {
          'VCCLCompilerTool': { 'ExceptionHandling': 1 },
        },
    }
  ]
}
