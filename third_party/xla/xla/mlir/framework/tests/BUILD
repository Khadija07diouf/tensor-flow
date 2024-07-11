load("//xla:lit.bzl", "enforce_glob", "lit_test_suite")

package(
    # copybara:uncomment default_applicable_licenses = ["//tensorflow:license"],
    licenses = ["notice"],
)

lit_test_suite(
    name = "all_tests",
    srcs = enforce_glob(
        [
            "legalize-xla-framework.mlir",
            "outline-with-xla-framework.mlir",
            "xla-framework.mlir",
        ],
        include = ["*.mlir"],
    ),
    cfg = "//xla:lit.cfg.py",
    tools = [
        "//third_party/llvm/llvm-project/llvm:FileCheck",
        "//xla/translate:xla-translate-opt",
    ],
)
