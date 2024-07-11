load(
    "@local_config_rocm//rocm:build_defs.bzl",
    "if_rocm_is_configured",
)
load(
    "@local_tsl//tsl/platform/default:cuda_build_defs.bzl",
    "if_cuda_is_configured",
)
load("//xla:xla.bzl", "xla_cc_test")
load("//xla/tsl:tsl.bzl", "internal_visibility")

package(
    # copybara:uncomment default_applicable_licenses = ["//tensorflow:license"],
    default_visibility = internal_visibility([":friends"]),
    licenses = ["notice"],
)

package_group(
    name = "friends",
    includes = [
        "//xla:friends",
    ],
)

cc_library(
    name = "llvm_gpu_backend",
    srcs = [
        "gpu_backend_lib.cc",
        "utils.cc",
    ],
    hdrs = [
        "gpu_backend_lib.h",
        "utils.h",
    ],
    local_defines = if_cuda_is_configured([
        "GOOGLE_CUDA=1",
    ]) + if_rocm_is_configured(["TENSORFLOW_USE_ROCM=1"]),
    deps = [
        "//third_party/llvm/llvm-project/llvm:Analysis",
        "//third_party/llvm/llvm-project/llvm:BitReader",
        "//third_party/llvm/llvm-project/llvm:BitWriter",
        "//third_party/llvm/llvm-project/llvm:CodeGen",
        "//third_party/llvm/llvm-project/llvm:Core",
        "//third_party/llvm/llvm-project/llvm:IPO",
        "//third_party/llvm/llvm-project/llvm:IRReader",
        "//third_party/llvm/llvm-project/llvm:Linker",
        "//third_party/llvm/llvm-project/llvm:MC",
        "//third_party/llvm/llvm-project/llvm:NVPTXCodeGen",  # buildcleaner: keep
        "//third_party/llvm/llvm-project/llvm:ObjCARC",  # buildcleaner: keep
        "//third_party/llvm/llvm-project/llvm:Passes",
        "//third_party/llvm/llvm-project/llvm:Scalar",
        "//third_party/llvm/llvm-project/llvm:Support",
        "//third_party/llvm/llvm-project/llvm:Target",
        "//third_party/llvm/llvm-project/mlir:NVVMDialect",
        "//xla:status_macros",
        "//xla:types",
        "//xla:util",
        "//xla:xla_proto_cc",
        "//xla/service/gpu:metrics",
        "//xla/service/llvm_ir:llvm_command_line_options",
        "//xla/service/llvm_ir:llvm_type_conversion_util",
        "//xla/stream_executor:device_description",
        "//xla/tsl/util:env_var",
        "@com_google_absl//absl/base",
        "@com_google_absl//absl/base:core_headers",
        "@com_google_absl//absl/container:flat_hash_map",
        "@com_google_absl//absl/memory",
        "@com_google_absl//absl/status",
        "@com_google_absl//absl/status:statusor",
        "@com_google_absl//absl/strings",
        "@com_google_absl//absl/strings:str_format",
        "@com_google_absl//absl/synchronization",
        "@local_config_cuda//cuda:cuda_headers",
        "@local_tsl//tsl/platform:cuda_libdevice_path",
        "@local_tsl//tsl/platform:env",
        "@local_tsl//tsl/platform:errors",
        "@local_tsl//tsl/platform:logging",
        "@local_tsl//tsl/platform:path",
        "@local_tsl//tsl/platform:random",
        "@local_tsl//tsl/platform:rocm_rocdl_path",
        "@local_tsl//tsl/platform:status",
        "@local_tsl//tsl/platform:statusor",
        "@local_tsl//tsl/profiler/lib:scoped_annotation",
        "@local_tsl//tsl/profiler/lib:traceme",
    ] + if_cuda_is_configured([
        "//xla/stream_executor/cuda:cuda_asm_compiler",
    ]) + if_rocm_is_configured([
        "@local_config_rocm//rocm:rocm_headers",
        "//third_party/llvm/llvm-project/llvm:AMDGPUCodeGen",
        "//third_party/llvm/llvm-project/llvm:AMDGPUAsmParser",
    ]),
)

xla_cc_test(
    name = "utils_test",
    size = "small",
    srcs = ["utils_test.cc"],
    data = [
        "tests_data/saxpy.ll",
    ],
    deps = [
        ":llvm_gpu_backend",
        "//third_party/llvm/llvm-project/llvm:Core",
        "//xla/tests:xla_internal_test_main",
        "@local_tsl//tsl/platform:path",
        "@local_tsl//tsl/platform:test",
    ],
)
