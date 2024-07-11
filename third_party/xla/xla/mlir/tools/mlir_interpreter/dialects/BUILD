load("@local_tsl//tsl/platform:rules_cc.bzl", "cc_library")

package(
    # copybara:uncomment default_applicable_licenses = ["//tensorflow:license"],
    default_visibility = [":friends"],
    licenses = ["notice"],
)

package_group(
    name = "friends",
    includes = [
        "//xla:friends",
    ],
)

cc_library(
    name = "dialects",
    srcs = glob(
        [
            "*.cc",
        ],
        exclude = ["util.cc"],
    ),
    deps = [
        ":dialect_utils",
        "//third_party/llvm/llvm-project/llvm:Support",
        "//third_party/llvm/llvm-project/mlir:AffineDialect",
        "//third_party/llvm/llvm-project/mlir:AffineUtils",
        "//third_party/llvm/llvm-project/mlir:ArithDialect",
        "//third_party/llvm/llvm-project/mlir:BufferizationDialect",
        "//third_party/llvm/llvm-project/mlir:ComplexDialect",
        "//third_party/llvm/llvm-project/mlir:FuncDialect",
        "//third_party/llvm/llvm-project/mlir:IR",
        "//third_party/llvm/llvm-project/mlir:LinalgDialect",
        "//third_party/llvm/llvm-project/mlir:MemRefDialect",
        "//third_party/llvm/llvm-project/mlir:SCFDialect",
        "//third_party/llvm/llvm-project/mlir:Support",
        "//third_party/llvm/llvm-project/mlir:TensorDialect",
        "//third_party/llvm/llvm-project/mlir:VectorDialect",
        "//xla/mlir/tools/mlir_interpreter/framework",
        "//xla/mlir_hlo",
    ],
    alwayslink = 1,
)

cc_library(
    name = "dialect_utils",
    srcs = [
        "util.cc",
    ],
    hdrs = [
        "comparators.h",
        "cwise_math.h",
        "util.h",
    ],
    deps = [
        "//third_party/llvm/llvm-project/llvm:Support",
        "//third_party/llvm/llvm-project/mlir:IR",
        "//third_party/llvm/llvm-project/mlir:Support",
        "//third_party/llvm/llvm-project/mlir:ViewLikeInterface",
        "//xla/mlir/tools/mlir_interpreter/framework",
    ],
)
