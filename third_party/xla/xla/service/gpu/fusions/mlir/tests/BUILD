load("//xla:lit.bzl", "lit_test_suite")
load("//xla:xla.bzl", "xla_cc_binary")

package(
    # copybara:uncomment default_applicable_licenses = ["//tensorflow:license"],
    licenses = ["notice"],
)

xla_cc_binary(
    name = "mlir_fusions_opt",
    srcs = ["mlir_fusions_opt.cc"],
    deps = [
        "//third_party/llvm/llvm-project/mlir:AffineDialect",
        "//third_party/llvm/llvm-project/mlir:ArithDialect",
        "//third_party/llvm/llvm-project/mlir:ComplexDialect",
        "//third_party/llvm/llvm-project/mlir:DLTIDialect",
        "//third_party/llvm/llvm-project/mlir:FuncDialect",
        "//third_party/llvm/llvm-project/mlir:FuncExtensions",
        "//third_party/llvm/llvm-project/mlir:GPUDialect",
        "//third_party/llvm/llvm-project/mlir:LLVMDialect",
        "//third_party/llvm/llvm-project/mlir:MathDialect",
        "//third_party/llvm/llvm-project/mlir:MlirOptLib",
        "//third_party/llvm/llvm-project/mlir:NVVMDialect",
        "//third_party/llvm/llvm-project/mlir:SCFDialect",
        "//third_party/llvm/llvm-project/mlir:Support",
        "//third_party/llvm/llvm-project/mlir:TensorDialect",
        "//third_party/llvm/llvm-project/mlir:Transforms",
        "//third_party/llvm/llvm-project/mlir:VectorDialect",
        "//xla/mlir_hlo",
        "//xla/service/gpu/fusions/mlir:passes",
        "//xla/service/gpu/fusions/mlir/ir:xla_gpu",
    ],
)

lit_test_suite(
    name = "tests",
    srcs = glob(["*.mlir"]),
    cfg = "//xla:lit.cfg.py",
    tools = [
        ":mlir_fusions_opt",
        "//third_party/llvm/llvm-project/llvm:FileCheck",
    ],
)
