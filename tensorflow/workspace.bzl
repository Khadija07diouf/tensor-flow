# TensorFlow external dependencies that can be loaded in WORKSPACE files.

# If TensorFlow is linked as a submodule, path_prefix is TensorFlow's directory
# within the workspace (e.g. "tensorflow/"), and tf_repo_name is the name of the
# local_repository rule (e.g. "@tf").
def tf_workspace(path_prefix = "", tf_repo_name = ""):
  native.new_http_archive(
    name = "gmock_archive",
    url = "https://github.com/google/googlemock/archive/release-1.7.0.zip",
    sha256 = "407992e9ef17a08339cd383c33dbaff923969cfa01f8e4ceaeea679400016d85",
    build_file = path_prefix + "google/protobuf/gmock.BUILD",
  )

  native.new_http_archive(
    name = "eigen_archive",
    url = "https://bitbucket.org/eigen/eigen/get/4b9c7d45d069.tar.gz",
    sha256 = "9228accc8e4a6e045ce6b1c28f2dbd0e09b9553f89e947db664f3c2d8e86c1e8",
    build_file = path_prefix + "eigen.BUILD",
  )

  native.bind(
    name = "gtest",
    actual = "@gmock_archive//:gtest",
  )

  native.bind(
    name = "gtest_main",
    actual = "@gmock_archive//:gtest_main",
  )

  native.git_repository(
    name = "re2",
    remote = "https://github.com/google/re2.git",
    commit = "791beff",
  )

  native.new_http_archive(
    name = "jpeg_archive",
    url = "http://www.ijg.org/files/jpegsrc.v9a.tar.gz",
    sha256 = "3a753ea48d917945dd54a2d97de388aa06ca2eb1066cbfdc6652036349fe05a7",
    build_file = path_prefix + "jpeg.BUILD",
  )

  native.new_http_archive(
    name = "png_archive",
    url = "https://github.com/glennrp/libpng/archive/v1.2.53.zip",
    sha256 = "c35bcc6387495ee6e757507a68ba036d38ad05b415c2553b3debe2a57647a692",
    build_file = path_prefix + "png.BUILD",
  )

  native.new_http_archive(
    name = "six_archive",
    url = "https://pypi.python.org/packages/source/s/six/six-1.10.0.tar.gz#md5=34eed507548117b2ab523ab14b2f8b55",
    sha256 = "105f8d68616f8248e24bf0e9372ef04d3cc10104f1980f54d57b2ce73a5ad56a",
    build_file = path_prefix + "six.BUILD",
  )

  native.bind(
    name = "six",
    actual = "@six_archive//:six",
  )

  # grpc expects //external:protobuf_clib and //external:protobuf_compiler
  # to point to the protobuf's compiler library.
  native.bind(
    name = "protobuf_clib",
    actual = tf_repo_name + "//google/protobuf:protoc_lib",
  )

  native.bind(
    name = "protobuf_compiler",
    actual = tf_repo_name + "//google/protobuf:protoc_lib",
  )

  native.git_repository(
    name = "grpc",
    commit = "3d62fc6",
    init_submodules = True,
    remote = "https://github.com/grpc/grpc.git",
  )

  # protobuf expects //external:grpc_cpp_plugin to point to grpc's
  # C++ plugin code generator.
  native.bind(
    name = "grpc_cpp_plugin",
    actual = "@grpc//:grpc_cpp_plugin",
  )

  native.bind(
    name = "grpc_lib",
    actual = "@grpc//:grpc++_unsecure",
  )
