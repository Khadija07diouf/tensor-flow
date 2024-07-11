load("@local_tsl//tsl/platform:rules_cc.bzl", "cc_library")
load("//third_party/llvm/llvm-project/mlir:tblgen.bzl", "gentbl_cc_library")
load("//xla/tsl:tsl.bzl", "internal_visibility")
load("//xla/tsl:tsl.default.bzl", "get_compatible_with_portable")

package(
    # copybara:uncomment default_applicable_licenses = ["//tensorflow:license"],
    default_visibility = internal_visibility(["//learning/brain/mlir:xla_friends"]),
    licenses = ["notice"],
)

gentbl_cc_library(
    name = "passes_inc_gen",
    compatible_with = get_compatible_with_portable(),
    tbl_outs = [
        (
            [
                "-gen-pass-decls",
                "-name=XlaFramework",
            ],
            "passes.h.inc",
        ),
    ],
    tblgen = "//third_party/llvm/llvm-project/mlir:mlir-tblgen",
    td_file = "passes.td",
    deps = [
        "//third_party/llvm/llvm-project/llvm:Support",
        "//third_party/llvm/llvm-project/mlir:FuncTdFiles",
        "//third_party/llvm/llvm-project/mlir:PassBaseTdFiles",
        "//third_party/llvm/llvm-project/mlir:TensorOpsTdFiles",
        "//xla/mlir_hlo:hlo_ops_td_files",
    ],
)

cc_library(
    name = "passes",
    srcs = [
        "outline_with_xla_framework.cc",
        "xla_framework_to_llvm_pass.cc",
    ],
    hdrs = [
        "passes.h",
    ],
    deps = [
        ":passes_inc_gen",
        "//third_party/llvm/llvm-project/llvm:Core",
        "//third_party/llvm/llvm-project/llvm:Support",
        "//third_party/llvm/llvm-project/mlir:FuncDialect",
        "//third_party/llvm/llvm-project/mlir:FuncToLLVM",
        "//third_party/llvm/llvm-project/mlir:IR",
        "//third_party/llvm/llvm-project/mlir:LLVMCommonConversion",
        "//third_party/llvm/llvm-project/mlir:LLVMDialect",
        "//third_party/llvm/llvm-project/mlir:Pass",
        "//third_party/llvm/llvm-project/mlir:Support",
        "//third_party/llvm/llvm-project/mlir:TransformUtils",
        "//xla/mlir/framework/ir:xla_framework",
    ],
)
