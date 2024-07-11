load("@local_tsl//tsl/platform:rules_cc.bzl", "cc_library")
load("//xla/tsl:tsl.bzl", "internal_visibility")

package(
    # copybara:uncomment default_applicable_licenses = ["//tensorflow:license"],
    default_visibility = internal_visibility([
        "//learning/brain/mlir:tensorflow_friends",
        "//learning/brain/mlir:xla_friends",
    ]),
    licenses = ["notice"],
)

cc_library(
    name = "translate",
    srcs = ["translate.cc"],
    hdrs = ["translate.h"],
    deps = [
        "//third_party/llvm/llvm-project/llvm:Support",
        "//third_party/llvm/llvm-project/mlir:FuncDialect",
        "//third_party/llvm/llvm-project/mlir:IR",
        "//third_party/llvm/llvm-project/mlir:Parser",
        "//third_party/llvm/llvm-project/mlir:Pass",
        "//third_party/llvm/llvm-project/mlir:Support",
        "//third_party/stablehlo:register",
        "//xla/mlir_hlo:mhlo_passes",
        "//xla/translate/mhlo_to_hlo:translate",
    ],
)

cc_library(
    name = "translate_registration",
    testonly = True,
    srcs = ["translate_registration.cc"],
    deps = [
        ":translate",
        "//third_party/llvm/llvm-project/llvm:Support",
        "//third_party/llvm/llvm-project/mlir:ArithDialect",
        "//third_party/llvm/llvm-project/mlir:FuncDialect",
        "//third_party/llvm/llvm-project/mlir:IR",
        "//third_party/llvm/llvm-project/mlir:Support",
        "//third_party/llvm/llvm-project/mlir:TensorDialect",
        "//third_party/llvm/llvm-project/mlir:TranslateLib",
        "//third_party/stablehlo:register",
    ],
    alwayslink = 1,
)
