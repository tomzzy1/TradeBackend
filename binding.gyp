{
  "targets": [
    {
      "target_name": "cppModule",
      "sources": [ "cpp_modules/node.cc"],
      'include_dirs': ["<!(node -p \"require('node-addon-api').include_dir\")",
      "C:/Program Files/MySQL/Connector C++ 8.0/include"],
      "libraries": ["mysqlcppconn8.lib"],
      'link_settings': {
        'library_dirs':["C:/Program Files/MySQL/Connector C++ 8.0/lib64/vs14"]
      },
      'msvs_settings': {
          'VCCLCompilerTool': { 'ExceptionHandling': 1 },
        },
      'conditions': [
        ['OS=="mac"', {
          'xcode_settings': {
            'GCC_ENABLE_CPP_EXCEPTIONS': 'YES'
          }
        }]
      ],
      "cflags!" : [
        "-fno-exceptions"
      ],
      "cflags_cc!": [
        "-fno-exceptions"
      ]
    }
  ]

}