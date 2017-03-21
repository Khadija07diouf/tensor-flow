# Template for popnn

package(default_visibility = ["//visibility:public"])

filegroup(
  name = "popnn_lib",
  srcs = glob(["lib/libpopnn.*"]),
)

filegroup(
  name = "popnn_static_lib",
  srcs = ["static_lib/libpopnn.a"],
)

cc_library(
  name = "popnn_libs",
  srcs = glob(["**/libpopnn.*"]),
)

cc_library(
  name = "popnn_headers",
  hdrs = glob(["**/*.h"]),
  includes = ["include"],
)

cc_library(
  name = "popnn",
  deps = [":popnn_libs", ":popnn_headers"]
)

