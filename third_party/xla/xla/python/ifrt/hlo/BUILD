load("//xla:xla.bzl", "xla_cc_test")
load("//xla/tsl:tsl.bzl", "internal_visibility")
load("//xla/tsl:tsl.default.bzl", "get_compatible_with_portable")

package(
    # copybara:uncomment default_applicable_licenses = ["//tensorflow:license"],
    default_visibility = internal_visibility([
        "//xla/python/ifrt:friends",
        "//xla/python/ifrt:internal",
    ]),
)

cc_library(
    name = "hlo_program",
    srcs = ["hlo_program.cc"],
    hdrs = ["hlo_program.h"],
    compatible_with = get_compatible_with_portable(),
    deps = [
        "//third_party/llvm/llvm-project/llvm:Support",
        "//third_party/llvm/llvm-project/mlir:IR",
        "//xla/python/ifrt",
    ],
)

cc_library(
    name = "hlo_program_serdes",
    srcs = ["hlo_program_serdes.cc"],
    compatible_with = get_compatible_with_portable(),
    deps = [
        ":hlo_program",
        "//third_party/llvm/llvm-project/llvm:Support",
        "//third_party/llvm/llvm-project/mlir:IR",
        "//third_party/llvm/llvm-project/mlir:Pass",
        "//third_party/llvm/llvm-project/mlir:Support",
        "//third_party/stablehlo:stablehlo_portable_api",
        "//third_party/stablehlo:stablehlo_serialization",
        "//xla/mlir/utils:error_util",
        "//xla/mlir_hlo:mhlo_passes",
        "//xla/pjrt:mlir_to_hlo",
        "//xla/python/ifrt:serdes",
        "@com_google_absl//absl/status",
        "@com_google_absl//absl/status:statusor",
        "@com_google_absl//absl/strings",
        "@local_tsl//tsl/platform:statusor",
    ],
    alwayslink = True,
)

xla_cc_test(
    name = "hlo_program_serdes_test",
    srcs = ["hlo_program_serdes_test.cc"],
    deps = [
        ":hlo_program",
        ":hlo_program_serdes",
        "//third_party/llvm/llvm-project/llvm:Support",
        "//third_party/llvm/llvm-project/mlir:FuncDialect",
        "//third_party/llvm/llvm-project/mlir:IR",
        "//third_party/llvm/llvm-project/mlir:Support",
        "//xla/mlir_hlo",
        "//xla/pjrt:mlir_to_hlo",
        "//xla/python/ifrt:serdes",
        "@com_google_absl//absl/log",
        "@com_google_absl//absl/status",
        "@com_google_absl//absl/strings:string_view",
        "@com_google_googletest//:gtest_main",
        "@local_tsl//tsl/platform:status_matchers",
        "@local_tsl//tsl/platform:statusor",
    ],
)
