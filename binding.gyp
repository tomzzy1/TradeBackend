{
  "targets": [
    {
      "target_name": "queryPricer",
      "sources": [ "cpp_modules/node.cc"],
      'include_dirs': ["<!(node -p \"require('node-addon-api').include_dir\")"],
      'msvs_settings': {
          'VCCLCompilerTool': { 'ExceptionHandling': 1 },
        }
    }
  ]

}