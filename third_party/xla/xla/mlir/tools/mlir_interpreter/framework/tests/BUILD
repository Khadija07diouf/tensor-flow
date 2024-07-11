load("//xla:xla.bzl", "xla_cc_test")

# copybara:uncomment package(default_applicable_licenses = ["//tensorflow:license"])

xla_cc_test(
    name = "tensor_or_memref_test",
    srcs = ["tensor_or_memref_test.cc"],
    deps = [
        "//third_party/llvm/llvm-project/llvm:Support",
        "//third_party/llvm/llvm-project/mlir:Support",
        "//xla/mlir/tools/mlir_interpreter/framework",
        "@com_google_absl//absl/strings",
        "@com_google_googletest//:gtest_main",
    ],
)

xla_cc_test(
    name = "interpreter_value_test",
    srcs = ["interpreter_value_test.cc"],
    deps = [
        "//third_party/llvm/llvm-project/llvm:Support",
        "//xla/mlir/tools/mlir_interpreter/framework",
        "@com_google_googletest//:gtest_main",
    ],
)
