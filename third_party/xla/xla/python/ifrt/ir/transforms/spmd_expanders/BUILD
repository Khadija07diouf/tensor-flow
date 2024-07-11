load("//xla/tsl:tsl.default.bzl", "get_compatible_with_portable")

package(
    # copybara:uncomment default_applicable_licenses = ["//tensorflow:license"],
    licenses = ["notice"],
)

cc_library(
    name = "spmd_expander",
    srcs = glob([
        "*spmd_expander.cc",
    ]),
    hdrs = glob([
        "*spmd_expander.h",
    ]),
    compatible_with = get_compatible_with_portable(),
    visibility = ["//xla/python/ifrt:friends"],
    deps = [
        "//third_party/llvm/llvm-project/llvm:Support",
        "//third_party/llvm/llvm-project/mlir:FuncDialect",
        "//third_party/llvm/llvm-project/mlir:IR",
        "//third_party/llvm/llvm-project/mlir:Support",
        "//xla/python/ifrt/ir",
        "//xla/python/ifrt/ir:sharding_param",
    ],
)
