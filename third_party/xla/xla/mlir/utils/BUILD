load("@local_tsl//tsl/platform:rules_cc.bzl", "cc_library")
load("//xla:xla.bzl", "xla_cc_test")
load("//xla/tsl:tsl.bzl", "internal_visibility")
load("//xla/tsl:tsl.default.bzl", "get_compatible_with_portable")

package(
    # copybara:uncomment default_applicable_licenses = ["//tensorflow:license"],
    default_visibility = internal_visibility([
        "//third_party/golang/github_com/gomlx/gomlx:__subpackages__",
        "//xla:internal",
    ]),
    licenses = ["notice"],
)

cc_library(
    name = "error_util",
    srcs = ["error_util.cc"],
    hdrs = ["error_util.h"],
    compatible_with = get_compatible_with_portable(),
    deps = [
        "//third_party/llvm/llvm-project/llvm:Support",
        "//third_party/llvm/llvm-project/mlir:IR",
        "@com_google_absl//absl/status",
        "@local_tsl//tsl/platform:errors",
    ],
)

cc_test(
    name = "error_util_test",
    srcs = ["error_util_test.cc"],
    deps = [
        ":error_util",
        "//third_party/llvm/llvm-project/llvm:Support",
        "//third_party/llvm/llvm-project/mlir:IR",
        "@com_google_absl//absl/status",
        "@com_google_absl//absl/strings",
        "@local_tsl//tsl/lib/core:status_test_util",
        "@local_tsl//tsl/platform:status",
        "@local_tsl//tsl/platform:test_main",
    ],
)

cc_library(
    name = "type_util",
    srcs = ["type_util.cc"],
    hdrs = ["type_util.h"],
    compatible_with = get_compatible_with_portable(),
    deps = [
        "//third_party/llvm/llvm-project/mlir:IR",
        "//third_party/llvm/llvm-project/mlir:Support",
        "//xla:shape_util",
        "//xla:util",
        "//xla:xla_data_proto_cc",
        "@com_google_absl//absl/status:statusor",
        "@local_tsl//tsl/platform:statusor",
    ],
)

xla_cc_test(
    name = "type_util_test",
    srcs = ["type_util_test.cc"],
    deps = [
        ":type_util",
        "//third_party/llvm/llvm-project/llvm:Support",
        "//third_party/llvm/llvm-project/mlir:IR",
        "//xla:shape_util",
        "//xla:xla_data_proto_cc",
        "@com_google_absl//absl/functional:function_ref",
        "@com_google_googletest//:gtest",
        "@local_tsl//tsl/platform:statusor",
        "@local_tsl//tsl/platform:test_main",
    ],
)
