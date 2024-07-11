# MLIR related utilities.

load(
    "@local_tsl//tsl/platform:rules_cc.bzl",
    "cc_library",
)

package(
    # copybara:uncomment default_applicable_licenses = ["//tensorflow:license"],
    default_visibility = [
        "//visibility:public",
    ],
    licenses = ["notice"],
)

cc_library(
    name = "status_scoped_diagnostic_handler",
    srcs = ["status_scoped_diagnostic_handler.cc"],
    hdrs = [
        "status_scoped_diagnostic_handler.h",
    ],
    deps = [
        "//third_party/llvm/llvm-project/llvm:Support",
        "//third_party/llvm/llvm-project/mlir:IR",
        "//third_party/llvm/llvm-project/mlir:Support",
        "@com_google_absl//absl/status",
        "@local_tsl//tsl/platform:logging",
    ],
)
