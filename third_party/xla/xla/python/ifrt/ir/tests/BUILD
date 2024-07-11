load("//xla:lit.bzl", "enforce_glob", "lit_test_suite")
load("//xla:xla.bzl", "xla_cc_binary", "xla_cc_test")

package(
    # copybara:uncomment default_applicable_licenses = ["//tensorflow:license"],
    licenses = ["notice"],
)

lit_test_suite(
    name = "all_tests",
    srcs = enforce_glob(
        [
            "ifrt_duplicated_callee_elimination.mlir",
            "ifrt_verify_donation.mlir",
            "ifrt_verify_sharding_specified.mlir",
            "spmd_expansion.mlir",
            "spmd_interface_verification.mlir",
            "verify_array.mlir",
            "verify_assemble.mlir",
            "verify_attrs.mlir",
            "verify_call.mlir",
            "verify_call_loaded_executable.mlir",
            "verify_copy_arrays.mlir",
            "verify_disassemble.mlir",
            "verify_loaded_executable.mlir",
            "verify_remap_arrays.mlir",
            "verify_reshard.mlir",
        ],
        include = [
            "*.mlir",
        ],
    ),
    cfg = "//xla:lit.cfg.py",
    tools = [
        ":ifrt-opt",
        "//third_party/llvm/llvm-project/llvm:FileCheck",
    ],
)

xla_cc_binary(
    name = "ifrt-opt",
    srcs = ["ifrt-opt.cc"],
    deps = [
        "//third_party/llvm/llvm-project/mlir:AllPassesAndDialects",
        "//third_party/llvm/llvm-project/mlir:IR",
        "//third_party/llvm/llvm-project/mlir:MlirOptLib",
        "//xla/mlir_hlo:hlo_dialect_registration",
        "//xla/python/ifrt/ir",
        "//xla/python/ifrt/ir/transforms:built_in_spmd_expansions",
        "//xla/python/ifrt/ir/transforms:passes",
    ],
)

cc_library(
    name = "executable_impl_test_base",
    testonly = True,
    srcs = ["executable_impl_test_base.cc"],
    hdrs = ["executable_impl_test_base.h"],
    visibility = ["//xla/python/ifrt:friends"],
    deps = [
        "//third_party/llvm/llvm-project/mlir:AllPassesAndDialects",
        "//third_party/llvm/llvm-project/mlir:IR",
        "//third_party/llvm/llvm-project/mlir:Parser",
        "//xla:status_macros",
        "//xla/mlir_hlo:hlo_dialect_registration",
        "//xla/python/ifrt",
        "//xla/python/ifrt:test_util",
        "//xla/python/ifrt/ir",
        "//xla/python/ifrt/ir:sharding_param",
        "//xla/python/ifrt/ir/transforms:built_in_spmd_expansions",
        "//xla/tsl/concurrency:ref_count",
        "@com_google_absl//absl/status:statusor",
        "@com_google_absl//absl/strings",
        "@com_google_absl//absl/types:span",
        "@local_tsl//tsl/platform:statusor",
        "@local_tsl//tsl/platform:test",
    ],
)

cc_library(
    name = "executable_impl_test_lib",
    testonly = True,
    srcs = ["executable_impl_test_lib.cc"],
    visibility = ["//xla/python/ifrt:friends"],
    deps = [
        ":executable_impl_test_base",
        "//third_party/llvm/llvm-project/llvm:Support",
        "//third_party/llvm/llvm-project/mlir:IR",
        "//xla/pjrt:pjrt_executable",
        "//xla/python/ifrt",
        "//xla/python/ifrt:test_util",
        "//xla/python/ifrt/hlo:hlo_program",
        "//xla/python/ifrt/ir:compiler",
        "//xla/python/ifrt/ir:sharding_param",
        "//xla/python/pjrt_ifrt:xla_ifrt",
        "//xla/service:computation_placer_hdr",
        "//xla/tsl/concurrency:ref_count",
        "@com_google_absl//absl/types:span",
        "@local_tsl//tsl/lib/core:status_test_util",
        "@local_tsl//tsl/platform:status_matchers",
        "@local_tsl//tsl/platform:statusor",
        "@local_tsl//tsl/platform:test",
    ],
    alwayslink = True,
)

xla_cc_test(
    name = "executable_test_no_impl",
    srcs = [],
    deps = [
        ":executable_impl_test_lib",
        "//xla/python/ifrt:no_impl_test_main",
    ],
)
