load("@local_tsl//tsl/platform:rules_cc.bzl", "cc_library")

# copybara:uncomment package(default_applicable_licenses = ["//tensorflow:license"])

cc_library(
    name = "rewrites",
    srcs = [
        "func.cc",
        "general.cc",
        "scf.cc",
    ],
    visibility = ["//visibility:public"],
    deps = [
        "//third_party/llvm/llvm-project/llvm:Support",
        "//third_party/llvm/llvm-project/mlir:ArithDialect",
        "//third_party/llvm/llvm-project/mlir:DialectUtils",
        "//third_party/llvm/llvm-project/mlir:FuncDialect",
        "//third_party/llvm/llvm-project/mlir:IR",
        "//third_party/llvm/llvm-project/mlir:SCFDialect",
        "//third_party/llvm/llvm-project/mlir:SideEffectInterfaces",
        "//third_party/llvm/llvm-project/mlir:Support",
        "//xla/mlir/tools/mlir_bisect:bisect_lib",
        "//xla/mlir/tools/mlir_replay/public:execution_trace_utils",
    ],
    alwayslink = 1,
)
