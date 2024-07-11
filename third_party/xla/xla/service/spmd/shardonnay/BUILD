# Description: Integrates Shardonnay into the current XLA pipeline.

load("//xla/tsl:tsl.bzl", "internal_visibility")
load("//xla/tsl:tsl.default.bzl", "get_compatible_with_libtpu_portable", "get_compatible_with_portable")

package(
    # copybara:uncomment default_applicable_licenses = ["//tensorflow:license"],
    default_visibility = internal_visibility(["//xla:internal"]),
    licenses = ["notice"],
)

cc_library(
    name = "constants",
    hdrs = ["constants.h"],
    compatible_with = get_compatible_with_portable() + get_compatible_with_libtpu_portable(),
    deps = ["//third_party/llvm/llvm-project/llvm:Support"],
)

cc_library(
    name = "utils",
    srcs = ["utils.cc"],
    hdrs = ["utils.h"],
    compatible_with = get_compatible_with_portable() + get_compatible_with_libtpu_portable(),
    deps = [
        ":constants",
        "//third_party/llvm/llvm-project/llvm:Support",
        "//third_party/llvm/llvm-project/mlir:FuncDialect",
        "//third_party/llvm/llvm-project/mlir:FuncExtensions",
        "//third_party/llvm/llvm-project/mlir:IR",
        "//third_party/llvm/llvm-project/mlir:Support",
        "//xla/mlir_hlo",
    ],
)
