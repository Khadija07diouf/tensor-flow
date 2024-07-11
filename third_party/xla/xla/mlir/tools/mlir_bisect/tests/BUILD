load("//xla:lit.bzl", "lit_test_suite")

# copybara:uncomment package(default_applicable_licenses = ["//tensorflow:license"])

lit_test_suite(
    name = "all_tests",
    srcs = glob(["*.mlir"]),
    cfg = "//xla:lit.cfg.py",
    data = [
        "snapshot.mlir.pb",
    ],
    tools = [
        "//third_party/llvm/llvm-project/llvm:FileCheck",
        "//third_party/llvm/llvm-project/llvm:not",
        "//xla/mlir/tools/mlir_bisect:mlir-bisect",
    ],
)
