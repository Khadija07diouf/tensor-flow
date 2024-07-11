load("//xla:lit.bzl", "enforce_glob", "lit_test_suite")
load("//xla/tsl:tsl.default.bzl", "filegroup")

package(
    # copybara:uncomment default_applicable_licenses = ["//tensorflow:license"],
    licenses = ["notice"],
)

lit_test_suite(
    name = "all_tests",
    srcs = enforce_glob(
        [
            "simple.mlir",
        ],
        include = [
            "*.mlir",
        ],
    ),
    cfg = "//xla:lit.cfg.py",
    data = [":test_utilities"],
    tools = [
        "//third_party/llvm/llvm-project/llvm:FileCheck",
        "//third_party/llvm/llvm-project/llvm:not",
        "//xla/translate:xla-translate",
    ],
)

# Bundle together all of the test utilities that are used by tests.
filegroup(
    name = "test_utilities",
    testonly = True,
)
