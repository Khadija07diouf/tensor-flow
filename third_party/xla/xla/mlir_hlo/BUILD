load("@local_tsl//tsl/platform:rules_cc.bzl", "cc_library")
load("//third_party/bazel_skylib/rules:build_test.bzl", "build_test")
load("//third_party/llvm/llvm-project/mlir:tblgen.bzl", "gentbl_cc_library", "gentbl_filegroup", "td_library")
load("//xla/tsl:tsl.bzl", "internal_visibility")
load("//xla/tsl:tsl.default.bzl", "get_compatible_with_portable")

package(
    # copybara:uncomment default_applicable_licenses = ["//tensorflow:license"],
    default_visibility = internal_visibility(["//learning/brain/mlir:mhlo_friends"]),
    licenses = ["notice"],
)

exports_files([
    "mhlo/IR/hlo_ops.td",
])

# Python extension sources.
exports_files(["bindings/python/MlirHloModule.cc"])

filegroup(
    name = "hlo_ops_td_filegroup",
    srcs = glob(["mhlo/IR/*.td"]),
)

td_library(
    name = "hlo_ops_td_files",
    srcs = glob(["mhlo/IR/*.td"]),
    compatible_with = get_compatible_with_portable(),
    includes = ["."],
    deps = [
        "//third_party/llvm/llvm-project/mlir:BuiltinDialectTdFiles",
        "//third_party/llvm/llvm-project/mlir:ControlFlowInterfacesTdFiles",
        "//third_party/llvm/llvm-project/mlir:CopyOpInterfaceTdFiles",
        "//third_party/llvm/llvm-project/mlir:InferTypeOpInterfaceTdFiles",
        "//third_party/llvm/llvm-project/mlir:LoopLikeInterfaceTdFiles",
        "//third_party/llvm/llvm-project/mlir:MemRefOpsTdFiles",
        "//third_party/llvm/llvm-project/mlir:OpBaseTdFiles",
        "//third_party/llvm/llvm-project/mlir:QuantizationOpsTdFiles",
        "//third_party/llvm/llvm-project/mlir:ShapeOpsTdFiles",
        "//third_party/llvm/llvm-project/mlir:SideEffectInterfacesTdFiles",
        "//third_party/llvm/llvm-project/mlir:ViewLikeInterfaceTdFiles",
        "//third_party/stablehlo:base_td_files",
        "//third_party/stablehlo:chlo_ops_td_files",
    ],
)

gentbl_cc_library(
    name = "mhlo_pass_inc_gen",
    compatible_with = get_compatible_with_portable(),
    strip_include_prefix = ".",
    tbl_outs = [
        (
            [
                "-gen-pass-decls",
                "-name=AllMhlo",
            ],
            "mhlo/transforms/mhlo_passes.h.inc",
        ),
    ],
    tblgen = "//third_party/llvm/llvm-project/mlir:mlir-tblgen",
    td_file = "mhlo/transforms/mhlo_passes.td",
    deps = ["//third_party/llvm/llvm-project/mlir:PassBaseTdFiles"],
)

gentbl_cc_library(
    name = "hlo_ops_inc_gen",
    compatible_with = get_compatible_with_portable(),
    strip_include_prefix = ".",
    tbl_outs = [
        (
            ["-gen-op-decls"],
            "mhlo/IR/hlo_ops.h.inc",
        ),
        (
            ["-gen-op-defs"],
            "mhlo/IR/hlo_ops.cc.inc",
        ),
    ],
    tblgen = "//third_party/llvm/llvm-project/mlir:mlir-tblgen",
    td_file = "mhlo/IR/hlo_ops.td",
    deps = [":hlo_ops_td_files"],
)

gentbl_cc_library(
    name = "hlo_ops_attrs_inc_gen",
    compatible_with = get_compatible_with_portable(),
    strip_include_prefix = ".",
    tbl_outs = [
        (
            ["-gen-attrdef-decls"],
            "mhlo/IR/hlo_ops_attrs.h.inc",
        ),
        (
            ["-gen-attrdef-defs"],
            "mhlo/IR/hlo_ops_attrs.cc.inc",
        ),
    ],
    tblgen = "//third_party/llvm/llvm-project/mlir:mlir-tblgen",
    td_file = "mhlo/IR/hlo_ops.td",
    deps = [":hlo_ops_td_files"],
)

gentbl_cc_library(
    name = "hlo_ops_enums_inc_gen",
    compatible_with = get_compatible_with_portable(),
    strip_include_prefix = ".",
    tbl_outs = [
        (
            ["-gen-enum-decls"],
            "mhlo/IR/hlo_ops_enums.h.inc",
        ),
        (
            ["-gen-enum-defs"],
            "mhlo/IR/hlo_ops_enums.cc.inc",
        ),
    ],
    tblgen = "//third_party/llvm/llvm-project/mlir:mlir-tblgen",
    td_file = "mhlo/IR/hlo_ops.td",
    deps = [":hlo_ops_td_files"],
)

gentbl_cc_library(
    name = "hlo_ops_typedefs_inc_gen",
    compatible_with = get_compatible_with_portable(),
    strip_include_prefix = ".",
    tbl_outs = [
        (
            [
                "-gen-typedef-decls",
                "--typedefs-dialect=mhlo",
            ],
            "mhlo/IR/hlo_ops_typedefs.h.inc",
        ),
        (
            [
                "-gen-typedef-defs",
                "--typedefs-dialect=mhlo",
            ],
            "mhlo/IR/hlo_ops_typedefs.cc.inc",
        ),
    ],
    tblgen = "//third_party/llvm/llvm-project/mlir:mlir-tblgen",
    td_file = "mhlo/IR/hlo_ops.td",
    deps = [":hlo_ops_td_files"],
)

gentbl_cc_library(
    name = "hlo_ops_pattern_inc_gen",
    compatible_with = get_compatible_with_portable(),
    strip_include_prefix = "mhlo/IR/",
    tbl_outs = [
        (
            ["-gen-rewriters"],
            "mhlo/IR/hlo_patterns.cc.inc",
        ),
    ],
    tblgen = "//third_party/llvm/llvm-project/mlir:mlir-tblgen",
    td_file = "mhlo/IR/hlo_patterns.td",
    deps = [
        ":hlo_ops_td_files",
        "//third_party/llvm/llvm-project/mlir:FuncTdFiles",
        "//third_party/llvm/llvm-project/mlir:TensorOpsTdFiles",
    ],
)

gentbl_filegroup(
    name = "hlo_ops_doc_gen",
    compatible_with = get_compatible_with_portable(),
    tbl_outs = [
        (
            [
                "-gen-dialect-doc",
                "-dialect=mhlo",
            ],
            "g3doc/hlo_ops.md",
        ),
    ],
    tblgen = "//third_party/llvm/llvm-project/mlir:mlir-tblgen",
    td_file = "mhlo/IR/hlo_ops.td",
    deps = [":hlo_ops_td_files"],
)

cc_library(
    name = "hlo_ops_common",
    srcs = ["mhlo/IR/hlo_ops_common.cc"],
    hdrs = ["mhlo/IR/hlo_ops_common.h"],
    strip_include_prefix = ".",
    deps = [
        "//third_party/llvm/llvm-project/llvm:Support",
        "//third_party/llvm/llvm-project/mlir:IR",
        "//third_party/llvm/llvm-project/mlir:Support",
    ],
)

#TODO(aminim): revisit the naming and grouping of these rules post-move.
gentbl_cc_library(
    name = "canonicalize_inc_gen",
    compatible_with = get_compatible_with_portable(),
    strip_include_prefix = ".",
    tbl_outs = [
        (
            ["-gen-rewriters"],
            "mhlo/IR/mhlo_canonicalize.inc",
        ),
    ],
    tblgen = "//third_party/llvm/llvm-project/mlir:mlir-tblgen",
    td_file = "mhlo/IR/mhlo_canonicalize.td",
    deps = [":hlo_ops_td_files"],
)

cc_library(
    name = "deallocation_passes",
    srcs = [
        "deallocation/transforms/buffer_reuse.cc",
    ],
    hdrs = [
        "deallocation/transforms/passes.h",
    ],
    strip_include_prefix = ".",
    deps = [
        ":deallocation_passes_inc_gen",
        ":deallocation_utils",
        "//third_party/llvm/llvm-project/llvm:Support",
        "//third_party/llvm/llvm-project/mlir:Analysis",
        "//third_party/llvm/llvm-project/mlir:ArithDialect",
        "//third_party/llvm/llvm-project/mlir:BufferizationDialect",
        "//third_party/llvm/llvm-project/mlir:ControlFlowInterfaces",
        "//third_party/llvm/llvm-project/mlir:FuncDialect",
        "//third_party/llvm/llvm-project/mlir:IR",
        "//third_party/llvm/llvm-project/mlir:LLVMCommonConversion",
        "//third_party/llvm/llvm-project/mlir:LLVMDialect",
        "//third_party/llvm/llvm-project/mlir:MemRefDialect",
        "//third_party/llvm/llvm-project/mlir:Pass",
        "//third_party/llvm/llvm-project/mlir:SCFDialect",
        "//third_party/llvm/llvm-project/mlir:Support",
        "//third_party/llvm/llvm-project/mlir:TransformUtils",
        "//third_party/llvm/llvm-project/mlir:Transforms",
    ],
)

gentbl_cc_library(
    name = "deallocation_passes_inc_gen",
    compatible_with = get_compatible_with_portable(),
    strip_include_prefix = ".",
    tbl_outs = [
        (
            [
                "-gen-pass-decls",
                "-name=Deallocation",
            ],
            "deallocation/transforms/passes.h.inc",
        ),
    ],
    tblgen = "//third_party/llvm/llvm-project/mlir:mlir-tblgen",
    td_file = "deallocation/transforms/passes.td",
    deps = ["//third_party/llvm/llvm-project/mlir:PassBaseTdFiles"],
)

cc_library(
    name = "deallocation_utils",
    srcs = ["deallocation/utils/util.cc"],
    hdrs = ["deallocation/utils/util.h"],
    strip_include_prefix = ".",
    deps = [
        "//third_party/llvm/llvm-project/llvm:Support",
        "//third_party/llvm/llvm-project/mlir:ControlFlowInterfaces",
        "//third_party/llvm/llvm-project/mlir:IR",
        "//third_party/llvm/llvm-project/mlir:MemRefDialect",
        "//third_party/llvm/llvm-project/mlir:SCFDialect",
    ],
)

cc_library(
    name = "convert_op_folder",
    srcs = ["utils/convert_op_folder.cc"],
    hdrs = ["utils/convert_op_folder.h"],
    strip_include_prefix = ".",
    deps = [
        "//third_party/llvm/llvm-project/llvm:Support",
        "//third_party/llvm/llvm-project/mlir:IR",
        "//third_party/llvm/llvm-project/mlir:Support",
    ],
)

cc_library(
    name = "mlir_hlo",
    srcs = [
        "mhlo/IR/hlo_ops.cc",
        "mhlo/IR/hlo_ops.cc.inc",
        "mhlo/IR/hlo_ops.h.inc",
        "mhlo/IR/hlo_ops_attrs.cc.inc",
        "mhlo/IR/hlo_ops_attrs.h.inc",
        "mhlo/IR/hlo_ops_enums.cc.inc",
        "mhlo/IR/hlo_ops_enums.h.inc",
        "mhlo/IR/hlo_ops_typedefs.cc.inc",
        "mhlo/IR/hlo_ops_typedefs.h.inc",
        "mhlo/IR/mhlo_bytecode.cc",
        "utils/hlo_utils.cc",
    ],
    hdrs = [
        "mhlo/IR/hlo_ops.h",
        "mhlo/IR/mhlo_bytecode.h",
        "utils/hlo_utils.h",
    ],
    strip_include_prefix = ".",
    deps = [
        ":canonicalize_inc_gen",
        ":convert_op_folder",
        ":hlo_ops_attrs_inc_gen",
        ":hlo_ops_common",
        ":hlo_ops_enums_inc_gen",
        ":hlo_ops_inc_gen",
        ":hlo_ops_pattern_inc_gen",
        ":hlo_ops_typedefs_inc_gen",
        "//third_party/llvm/llvm-project/llvm:Support",
        "//third_party/llvm/llvm-project/mlir:Analysis",
        "//third_party/llvm/llvm-project/mlir:ArithDialect",
        "//third_party/llvm/llvm-project/mlir:BytecodeOpInterface",
        "//third_party/llvm/llvm-project/mlir:ComplexDialect",
        "//third_party/llvm/llvm-project/mlir:ControlFlowInterfaces",
        "//third_party/llvm/llvm-project/mlir:Dialect",
        "//third_party/llvm/llvm-project/mlir:FuncDialect",
        "//third_party/llvm/llvm-project/mlir:FunctionInterfaces",
        "//third_party/llvm/llvm-project/mlir:IR",
        "//third_party/llvm/llvm-project/mlir:InferTypeOpInterface",
        "//third_party/llvm/llvm-project/mlir:InliningUtils",
        "//third_party/llvm/llvm-project/mlir:LLVMDialect",
        "//third_party/llvm/llvm-project/mlir:MemRefDialect",
        "//third_party/llvm/llvm-project/mlir:Pass",
        "//third_party/llvm/llvm-project/mlir:QuantOps",
        "//third_party/llvm/llvm-project/mlir:ShapeDialect",
        "//third_party/llvm/llvm-project/mlir:SideEffectInterfaces",
        "//third_party/llvm/llvm-project/mlir:SparseTensorDialect",
        "//third_party/llvm/llvm-project/mlir:Support",
        "//third_party/llvm/llvm-project/mlir:TensorDialect",
        "//third_party/llvm/llvm-project/mlir:TransformUtils",
        "//third_party/llvm/llvm-project/mlir:Transforms",
        "//third_party/stablehlo:base",
        "//third_party/stablehlo:broadcast_utils",
        "//third_party/stablehlo:chlo_ops",
        "//third_party/stablehlo:stablehlo_assembly_format",
        "//third_party/stablehlo:stablehlo_type_inference",
    ],
)

cc_library(
    name = "hlo_dialect_registration",
    srcs = ["mhlo/IR/init.cc"],
    hdrs = ["mhlo/IR/register.h"],
    strip_include_prefix = ".",
    deps = [
        ":mlir_hlo",
        "//third_party/llvm/llvm-project/mlir:IR",
        "//third_party/llvm/llvm-project/mlir:SparseTensorDialect",
        # Backward compatibility with the old way of registering CHLO dialect
        "//third_party/stablehlo:chlo_ops",
    ],
)

cc_library(
    name = "mhlo_passes",
    srcs = [
        "mhlo/transforms/broadcast_propagation/broadcast_propagation.cc",
        "mhlo/transforms/chlo_legalize_to_hlo/chlo_legalize_to_hlo_pass.cc",
        "mhlo/transforms/collapse_elementwise_map/collapse_elementwise_map.cc",
        "mhlo/transforms/constraint_fusion/constraint_fusion_pass.cc",
        "mhlo/transforms/convert_to_signless/convert_to_signless_pass.cc",
        "mhlo/transforms/expand_hlo_tuples/expand_hlo_tuples.cc",
        "mhlo/transforms/expand_ops_simplifier/expand_ops_simplifier.cc",
        "mhlo/transforms/group_reduction_dimensions/group_reduction_dimensions.cc",
        "mhlo/transforms/hlo_legalize_to_arithmetic/hlo_legalize_to_arithmetic.cc",
        "mhlo/transforms/hlo_legalize_to_memref/hlo_legalize_to_memref.cc",
        "mhlo/transforms/hlo_legalize_to_stablehlo/hlo_legalize_to_stablehlo_pass.cc",
        "mhlo/transforms/legalize_broadcast_to_broadcast_in_dim/legalize_broadcast_to_broadcast_in_dim.cc",
        "mhlo/transforms/legalize_control_flow/legalize_control_flow.cc",
        "mhlo/transforms/legalize_create_token_to_after_all/legalize_create_token_to_after_all.cc",
        "mhlo/transforms/legalize_cross_replica_sum_to_all_reduce/legalize_cross_replica_sum_to_all_reduce.cc",
        "mhlo/transforms/legalize_dot_general_to_dot/legalize_dot_general_to_dot.cc",
        "mhlo/transforms/legalize_dot_to_dot_general/legalize_dot_to_dot_general.cc",
        "mhlo/transforms/legalize_einsum_to_dot_general/legalize_einsum_to_dot_general.cc",
        "mhlo/transforms/legalize_gather_to_torch_index_select/legalize_gather_to_torch_index_select.cc",
        "mhlo/transforms/legalize_shape_computations/legalize_shape_computations.cc",
        "mhlo/transforms/legalize_sort/legalize_sort.cc",
        "mhlo/transforms/legalize_to_linalg/legalize_to_linalg.cc",
        "mhlo/transforms/legalize_to_standard/generated_legalize_to_standard.inc",
        "mhlo/transforms/legalize_to_standard/legalize_to_standard.cc",
        "mhlo/transforms/legalize_torch_index_select_to_gather/legalize_torch_index_select_to_gather.cc",
        "mhlo/transforms/legalize_trigonometric_to_approximation/legalize_trigonometric_to_approximation.cc",
        "mhlo/transforms/lower_complex/generated_lower_complex.inc",
        "mhlo/transforms/lower_complex/lower_complex.cc",
        "mhlo/transforms/lower_general_dot/lower_general_dot.cc",
        "mhlo/transforms/materialize_broadcasts/materialize_broadcasts.cc",
        "mhlo/transforms/materialize_broadcasts/materialize_broadcasts_pass.cc",
        "mhlo/transforms/merge_assuming_ops/merge_assuming_ops.cc",
        "mhlo/transforms/mhlo_canonicalize_dot/mhlo_canonicalize_dot.cc",
        "mhlo/transforms/mhlo_canonicalize_gather/mhlo_canonicalize_gather.cc",
        "mhlo/transforms/mhlo_canonicalize_reduction/mhlo_canonicalize_reduction.cc",
        "mhlo/transforms/mhlo_canonicalize_scatter/mhlo_canonicalize_scatter.cc",
        "mhlo/transforms/mhlo_flatten_tuple/mhlo_flatten_tuple.cc",
        "mhlo/transforms/mhlo_passes.h.inc",
        "mhlo/transforms/optimize_mhlo/optimize_mhlo.cc",
        "mhlo/transforms/optimize_mhlo/optimize_mhlo_pass.cc",
        "mhlo/transforms/prepare_for_export/prepare_for_export.cc",
        "mhlo/transforms/restrict_max_rank/restrict_max_rank.cc",
        "mhlo/transforms/shape_legalize_to_hlo/shape_legalize_to_hlo.cc",
        "mhlo/transforms/shape_reification/shape_reification_pass.cc",
        "mhlo/transforms/shape_simplification/shape_simplification.cc",
        "mhlo/transforms/sink_constants_to_control_flow/sink_constants_to_control_flow.cc",
        "mhlo/transforms/stablehlo_legalize_to_hlo/stablehlo_legalize_to_hlo_pass.cc",
        "mhlo/transforms/symbolic_shape_optimization/symbolic_shape_optimization.cc",
        "mhlo/transforms/test_infer_shaped_type/test_infer_shaped_type_pass.cc",
        "mhlo/transforms/unfuse_batch_norm/unfuse_batch_norm_pass.cc",
    ],
    hdrs = [
        "mhlo/interfaces/bufferizable_op_interface_impl.h",
        "mhlo/transforms/passes.h",
        "mhlo/transforms/rewriters.h",
        "mhlo/utils/legalize_to_linalg_utils.h",
        "mhlo/utils/mhlo_rng_utils.h",
    ],
    strip_include_prefix = ".",
    deps = [
        ":chlo_legalize_to_hlo_inc_gen",
        ":hlo_legalize_to_stablehlo",
        ":legalize_to_linalg_utils",
        ":legalize_to_standard_inc_gen",
        ":lower_complex_inc_gen",
        ":map_chlo_to_hlo_op",
        ":map_mhlo_to_scalar_op",
        ":mhlo_pass_inc_gen",
        ":mhlo_rng_utils",
        ":mhlo_scatter_gather_utils",
        ":mlir_hlo",
        ":shape_component_analysis",
        ":stablehlo_legalize_to_hlo",
        ":type_conversion",
        ":unfuse_batch_norm",
        "//third_party/llvm/llvm-project/llvm:Support",
        "//third_party/llvm/llvm-project/mlir:AffineDialect",
        "//third_party/llvm/llvm-project/mlir:ArithDialect",
        "//third_party/llvm/llvm-project/mlir:ArithUtils",
        "//third_party/llvm/llvm-project/mlir:BufferizationDialect",
        "//third_party/llvm/llvm-project/mlir:BufferizationInterfaces",
        "//third_party/llvm/llvm-project/mlir:BufferizationTransforms",
        "//third_party/llvm/llvm-project/mlir:ComplexDialect",
        "//third_party/llvm/llvm-project/mlir:ControlFlowDialect",
        "//third_party/llvm/llvm-project/mlir:DialectUtils",
        "//third_party/llvm/llvm-project/mlir:FuncDialect",
        "//third_party/llvm/llvm-project/mlir:FuncTransforms",
        "//third_party/llvm/llvm-project/mlir:IR",
        "//third_party/llvm/llvm-project/mlir:InferTypeOpInterface",
        "//third_party/llvm/llvm-project/mlir:LinalgDialect",
        "//third_party/llvm/llvm-project/mlir:LinalgTransforms",
        "//third_party/llvm/llvm-project/mlir:LinalgUtils",
        "//third_party/llvm/llvm-project/mlir:MathDialect",
        "//third_party/llvm/llvm-project/mlir:MemRefDialect",
        "//third_party/llvm/llvm-project/mlir:Pass",
        "//third_party/llvm/llvm-project/mlir:QuantOps",
        "//third_party/llvm/llvm-project/mlir:SCFDialect",
        "//third_party/llvm/llvm-project/mlir:ShapeDialect",
        "//third_party/llvm/llvm-project/mlir:ShapeTransforms",
        "//third_party/llvm/llvm-project/mlir:SideEffectInterfaces",
        "//third_party/llvm/llvm-project/mlir:SparseTensorDialect",
        "//third_party/llvm/llvm-project/mlir:Support",
        "//third_party/llvm/llvm-project/mlir:TensorDialect",
        "//third_party/llvm/llvm-project/mlir:TensorUtils",
        "//third_party/llvm/llvm-project/mlir:TransformUtils",
        "//third_party/llvm/llvm-project/mlir:Transforms",
        "//third_party/stablehlo:base",
        "//third_party/stablehlo:chlo_ops",
        "//third_party/stablehlo:stablehlo_ops",
        "//third_party/stablehlo:stablehlo_passes",
    ],
)

cc_library(
    name = "type_conversion",
    srcs = ["mhlo/utils/type_conversion.cc"],
    hdrs = ["mhlo/utils/type_conversion.h"],
    strip_include_prefix = ".",
    deps = [
        ":mlir_hlo",
        "//third_party/llvm/llvm-project/mlir:FuncDialect",
        "//third_party/llvm/llvm-project/mlir:FuncTransforms",
        "//third_party/llvm/llvm-project/mlir:IR",
        "//third_party/llvm/llvm-project/mlir:Support",
        "//third_party/llvm/llvm-project/mlir:TensorDialect",
        "//third_party/llvm/llvm-project/mlir:TransformUtils",
        "//third_party/llvm/llvm-project/mlir:Transforms",
        "//third_party/stablehlo:stablehlo_ops",
    ],
)

cc_library(
    name = "map_mhlo_to_scalar_op",
    hdrs = ["mhlo/transforms/map_mhlo_to_scalar_op.h"],
    strip_include_prefix = ".",
    deps = [
        ":mlir_hlo",
        "//third_party/llvm/llvm-project/llvm:Support",
        "//third_party/llvm/llvm-project/mlir:ArithDialect",
        "//third_party/llvm/llvm-project/mlir:ComplexDialect",
        "//third_party/llvm/llvm-project/mlir:IR",
        "//third_party/llvm/llvm-project/mlir:MathDialect",
        "//third_party/llvm/llvm-project/mlir:SCFDialect",
        "//third_party/llvm/llvm-project/mlir:Support",
    ],
)

cc_library(
    name = "map_chlo_to_hlo_op",
    hdrs = ["mhlo/transforms/map_chlo_to_hlo_op.h"],
    strip_include_prefix = ".",
    deps = [
        ":mlir_hlo",
        "//third_party/llvm/llvm-project/mlir:IR",
        "//third_party/stablehlo:chlo_ops",
    ],
)

cc_library(
    name = "map_stablehlo_to_hlo_op",
    hdrs = ["mhlo/transforms/map_stablehlo_to_hlo_op.h"],
    strip_include_prefix = ".",
    deps = [
        ":mlir_hlo",
        "//third_party/stablehlo:stablehlo_ops",
    ],
)

cc_library(
    name = "codegen_utils",
    srcs = ["utils/codegen_utils.cc"],
    hdrs = ["utils/codegen_utils.h"],
    strip_include_prefix = ".",
    deps = [
        "//third_party/llvm/llvm-project/llvm:Support",
        "//third_party/llvm/llvm-project/mlir:ArithDialect",
        "//third_party/llvm/llvm-project/mlir:FuncDialect",
        "//third_party/llvm/llvm-project/mlir:IR",
        "//third_party/llvm/llvm-project/mlir:LLVMDialect",
        "//third_party/llvm/llvm-project/mlir:MemRefDialect",
        "//third_party/llvm/llvm-project/mlir:Pass",
        "//third_party/llvm/llvm-project/mlir:Support",
    ],
)

cc_library(
    name = "placement_utils",
    hdrs = ["utils/placement_utils.h"],
    strip_include_prefix = ".",
    deps = ["//third_party/llvm/llvm-project/llvm:Support"],
)

cc_library(
    name = "legalize_to_linalg_utils",
    srcs = ["mhlo/utils/legalize_to_linalg_utils.cc"],
    hdrs = ["mhlo/utils/legalize_to_linalg_utils.h"],
    strip_include_prefix = ".",
    deps = [
        ":map_mhlo_to_scalar_op",
        ":mlir_hlo",
        "//third_party/llvm/llvm-project/llvm:Support",
        "//third_party/llvm/llvm-project/mlir:AffineDialect",
        "//third_party/llvm/llvm-project/mlir:BufferizationDialect",
        "//third_party/llvm/llvm-project/mlir:ComplexDialect",
        "//third_party/llvm/llvm-project/mlir:FuncDialect",
        "//third_party/llvm/llvm-project/mlir:IR",
        "//third_party/llvm/llvm-project/mlir:LinalgDialect",
        "//third_party/llvm/llvm-project/mlir:LinalgUtils",
        "//third_party/llvm/llvm-project/mlir:MathDialect",
        "//third_party/llvm/llvm-project/mlir:ShapeDialect",
        "//third_party/llvm/llvm-project/mlir:SparseTensorDialect",
        "//third_party/llvm/llvm-project/mlir:Support",
        "//third_party/llvm/llvm-project/mlir:TensorDialect",
        "//third_party/llvm/llvm-project/mlir:TensorUtils",
        "//third_party/llvm/llvm-project/mlir:TransformUtils",
        "//third_party/llvm/llvm-project/mlir:Transforms",
        "//third_party/stablehlo:chlo_ops",
    ],
)

cc_library(
    name = "mhlo_rng_utils",
    srcs = ["mhlo/utils/mhlo_rng_utils.cc"],
    hdrs = ["mhlo/utils/mhlo_rng_utils.h"],
    strip_include_prefix = ".",
    deps = [
        ":mlir_hlo",
        "//third_party/llvm/llvm-project/llvm:Support",
        "//third_party/llvm/llvm-project/mlir:ArithDialect",
        "//third_party/llvm/llvm-project/mlir:DialectUtils",
        "//third_party/llvm/llvm-project/mlir:IR",
        "//third_party/llvm/llvm-project/mlir:LinalgDialect",
        "//third_party/llvm/llvm-project/mlir:MathDialect",
        "//third_party/llvm/llvm-project/mlir:Support",
        "//third_party/llvm/llvm-project/mlir:TensorDialect",
        "//third_party/llvm/llvm-project/mlir:Transforms",
    ],
)

cc_library(
    name = "mhlo_scatter_gather_utils",
    srcs = ["mhlo/utils/mhlo_scatter_gather_utils.cc"],
    hdrs = ["mhlo/utils/mhlo_scatter_gather_utils.h"],
    strip_include_prefix = ".",
    deps = [
        ":mlir_hlo",
        "//third_party/llvm/llvm-project/mlir:DialectUtils",
        "//third_party/llvm/llvm-project/mlir:Support",
        "//third_party/llvm/llvm-project/mlir:TensorDialect",
    ],
)

gentbl_cc_library(
    name = "legalize_to_standard_inc_gen",
    compatible_with = get_compatible_with_portable(),
    strip_include_prefix = "mhlo/transforms/",
    tbl_outs = [
        (
            ["-gen-rewriters"],
            "mhlo/transforms/legalize_to_standard/generated_legalize_to_standard.inc",
        ),
    ],
    tblgen = "//third_party/llvm/llvm-project/mlir:mlir-tblgen",
    td_file = "mhlo/transforms/legalize_to_standard/legalize_to_standard_patterns.td",
    deps = [
        ":hlo_ops_td_files",
        "//third_party/llvm/llvm-project/mlir:ArithOpsTdFiles",
        "//third_party/llvm/llvm-project/mlir:FuncTdFiles",
        "//third_party/llvm/llvm-project/mlir:MathOpsTdFiles",
    ],
)

gentbl_cc_library(
    name = "lower_complex_inc_gen",
    compatible_with = get_compatible_with_portable(),
    strip_include_prefix = "mhlo/transforms/",
    tbl_outs = [
        (
            ["-gen-rewriters"],
            "mhlo/transforms/lower_complex/generated_lower_complex.inc",
        ),
    ],
    tblgen = "//third_party/llvm/llvm-project/mlir:mlir-tblgen",
    td_file = "mhlo/transforms/lower_complex/lower_complex_patterns.td",
    deps = [
        ":hlo_ops_td_files",
        "//third_party/llvm/llvm-project/mlir:FuncTdFiles",
    ],
)

cc_library(
    name = "unfuse_batch_norm",
    srcs = ["mhlo/transforms/unfuse_batch_norm/unfuse_batch_norm.cc"],
    hdrs = ["mhlo/transforms/rewriters.h"],
    strip_include_prefix = ".",
    deps = [
        ":mlir_hlo",
        "//third_party/llvm/llvm-project/llvm:Support",
        "//third_party/llvm/llvm-project/mlir:ArithDialect",
        "//third_party/llvm/llvm-project/mlir:FuncDialect",
        "//third_party/llvm/llvm-project/mlir:IR",
        "//third_party/llvm/llvm-project/mlir:ShapeDialect",
        "//third_party/llvm/llvm-project/mlir:Support",
        "//third_party/llvm/llvm-project/mlir:TensorDialect",
        "//third_party/llvm/llvm-project/mlir:TransformUtils",
        "//third_party/llvm/llvm-project/mlir:Transforms",
    ],
)

gentbl_cc_library(
    name = "chlo_legalize_to_hlo_inc_gen",
    compatible_with = get_compatible_with_portable(),
    strip_include_prefix = "mhlo/transforms",
    tbl_outs = [
        (
            ["-gen-rewriters"],
            "mhlo/transforms/chlo_legalize_to_hlo/generated_chlo_legalize_to_hlo.inc",
        ),
    ],
    tblgen = "//third_party/llvm/llvm-project/mlir:mlir-tblgen",
    td_file = "mhlo/transforms/chlo_legalize_to_hlo/chlo_legalize_to_hlo_patterns.td",
    deps = [":hlo_ops_td_files"],
)

cc_library(
    name = "hlo_legalize_to_stablehlo",
    srcs = ["mhlo/transforms/hlo_legalize_to_stablehlo/hlo_legalize_to_stablehlo.cc"],
    hdrs = ["mhlo/transforms/rewriters.h"],
    strip_include_prefix = ".",
    deps = [
        ":map_stablehlo_to_hlo_op",
        ":mlir_hlo",
        "//third_party/llvm/llvm-project/llvm:Support",
        "//third_party/llvm/llvm-project/mlir:FuncDialect",
        "//third_party/llvm/llvm-project/mlir:IR",
        "//third_party/llvm/llvm-project/mlir:Support",
        "//third_party/llvm/llvm-project/mlir:TransformUtils",
        "//third_party/llvm/llvm-project/mlir:Transforms",
        "//third_party/stablehlo:stablehlo_ops",
        "//third_party/stablehlo:stablehlo_ops_inc_gen",
    ],
)

cc_library(
    name = "stablehlo_legalize_to_hlo",
    srcs = ["mhlo/transforms/stablehlo_legalize_to_hlo/stablehlo_legalize_to_hlo.cc"],
    hdrs = ["mhlo/transforms/rewriters.h"],
    strip_include_prefix = ".",
    deps = [
        ":map_stablehlo_to_hlo_op",
        ":mlir_hlo",
        "//third_party/llvm/llvm-project/llvm:Support",
        "//third_party/llvm/llvm-project/mlir:AsmParser",
        "//third_party/llvm/llvm-project/mlir:FuncDialect",
        "//third_party/llvm/llvm-project/mlir:IR",
        "//third_party/llvm/llvm-project/mlir:Support",
        "//third_party/llvm/llvm-project/mlir:TransformUtils",
        "//third_party/llvm/llvm-project/mlir:Transforms",
        "//third_party/stablehlo:stablehlo_ops",
        "//third_party/stablehlo:stablehlo_ops_inc_gen",
    ],
)

cc_library(
    name = "all_passes",
    srcs = [
        # These are not exposed as headers in the dependent targets, and
        # shouldn't be. Ideally, this entire target should be removed.
        "deallocation/transforms/passes.h.inc",
        "transforms/passes.h.inc",
    ],
    hdrs = [
        "deallocation/transforms/passes.h",
        "mhlo/transforms/passes.h",
        "transforms/passes.h",
    ],
    strip_include_prefix = ".",
    deps = [
        ":deallocation_passes",
        ":deallocation_passes_inc_gen",
        ":mhlo_pass_inc_gen",
        ":mhlo_passes",
        ":stablehlo_legalize_to_hlo",
        ":transforms_passes",
        ":transforms_passes_inc_gen",
        "//third_party/llvm/llvm-project/llvm:Support",
        "//third_party/llvm/llvm-project/mlir:FuncDialect",
        "//third_party/llvm/llvm-project/mlir:IR",
        "//third_party/llvm/llvm-project/mlir:LLVMCommonConversion",
        "//third_party/llvm/llvm-project/mlir:LLVMDialect",
        "//third_party/llvm/llvm-project/mlir:Pass",
    ],
)

cc_library(
    name = "transforms_passes",
    srcs = [
        "transforms/alloc_to_arg_pass.cc",
        "transforms/bufferize.cc",
        "transforms/bufferize_pass.cc",
        "transforms/collapse_parallel_loops_to_1d_pass.cc",
        "transforms/detensorize_scf_ops.cc",
        "transforms/generic_host_to_llvm.cc",
        "transforms/lower_index_cast_pass.cc",
        "transforms/naive_copy_removal.cc",
        "transforms/tile_loops_pass.cc",
        "transforms/unbufferize_pass.cc",
        "transforms/vectorize_copy.cc",
    ],
    hdrs = [
        "transforms/passes.h",
        "transforms/passes.h.inc",
        "transforms/rewriters.h",
    ],
    strip_include_prefix = ".",
    deps = [
        ":deallocation_passes",
        ":mhlo_passes",
        ":mlir_hlo",
        ":shape_component_analysis",
        ":transforms_passes_inc_gen",
        ":type_conversion",
        "//third_party/llvm/llvm-project/llvm:Support",
        "//third_party/llvm/llvm-project/mlir:AffineDialect",
        "//third_party/llvm/llvm-project/mlir:AffineToStandard",
        "//third_party/llvm/llvm-project/mlir:Analysis",
        "//third_party/llvm/llvm-project/mlir:ArithDialect",
        "//third_party/llvm/llvm-project/mlir:ArithToLLVM",
        "//third_party/llvm/llvm-project/mlir:ArithTransforms",
        "//third_party/llvm/llvm-project/mlir:ArithUtils",
        "//third_party/llvm/llvm-project/mlir:AsmParser",
        "//third_party/llvm/llvm-project/mlir:BufferizationDialect",
        "//third_party/llvm/llvm-project/mlir:BufferizationInterfaces",
        "//third_party/llvm/llvm-project/mlir:BufferizationTransforms",
        "//third_party/llvm/llvm-project/mlir:ComplexDialect",
        "//third_party/llvm/llvm-project/mlir:ComplexToLLVM",
        "//third_party/llvm/llvm-project/mlir:ControlFlowDialect",
        "//third_party/llvm/llvm-project/mlir:ControlFlowToLLVM",
        "//third_party/llvm/llvm-project/mlir:CopyOpInterface",
        "//third_party/llvm/llvm-project/mlir:DialectUtils",
        "//third_party/llvm/llvm-project/mlir:FuncDialect",
        "//third_party/llvm/llvm-project/mlir:FuncToLLVM",
        "//third_party/llvm/llvm-project/mlir:FuncTransforms",
        "//third_party/llvm/llvm-project/mlir:GPUDialect",
        "//third_party/llvm/llvm-project/mlir:IR",
        "//third_party/llvm/llvm-project/mlir:LLVMCommonConversion",
        "//third_party/llvm/llvm-project/mlir:LLVMDialect",
        "//third_party/llvm/llvm-project/mlir:LinalgDialect",
        "//third_party/llvm/llvm-project/mlir:LinalgTransforms",
        "//third_party/llvm/llvm-project/mlir:LoopLikeInterface",
        "//third_party/llvm/llvm-project/mlir:MathDialect",
        "//third_party/llvm/llvm-project/mlir:MathToLLVM",
        "//third_party/llvm/llvm-project/mlir:MathToLibm",
        "//third_party/llvm/llvm-project/mlir:MathTransforms",
        "//third_party/llvm/llvm-project/mlir:MemRefDialect",
        "//third_party/llvm/llvm-project/mlir:MemRefToLLVM",
        "//third_party/llvm/llvm-project/mlir:MemRefTransforms",
        "//third_party/llvm/llvm-project/mlir:MemRefUtils",
        "//third_party/llvm/llvm-project/mlir:NVVMDialect",
        "//third_party/llvm/llvm-project/mlir:PDLDialect",
        "//third_party/llvm/llvm-project/mlir:Pass",
        "//third_party/llvm/llvm-project/mlir:ReconcileUnrealizedCasts",
        "//third_party/llvm/llvm-project/mlir:Rewrite",
        "//third_party/llvm/llvm-project/mlir:SCFDialect",
        "//third_party/llvm/llvm-project/mlir:SCFToControlFlow",
        "//third_party/llvm/llvm-project/mlir:SCFTransforms",
        "//third_party/llvm/llvm-project/mlir:SCFUtils",
        "//third_party/llvm/llvm-project/mlir:ShapeDialect",
        "//third_party/llvm/llvm-project/mlir:ShapeTransforms",
        "//third_party/llvm/llvm-project/mlir:Support",
        "//third_party/llvm/llvm-project/mlir:TensorDialect",
        "//third_party/llvm/llvm-project/mlir:TensorTransforms",
        "//third_party/llvm/llvm-project/mlir:TensorUtils",
        "//third_party/llvm/llvm-project/mlir:TransformDialect",
        "//third_party/llvm/llvm-project/mlir:TransformDialectInterfaces",
        "//third_party/llvm/llvm-project/mlir:TransformDialectTransforms",
        "//third_party/llvm/llvm-project/mlir:TransformUtils",
        "//third_party/llvm/llvm-project/mlir:Transforms",
        "//third_party/llvm/llvm-project/mlir:VectorDialect",
        "//third_party/llvm/llvm-project/mlir:VectorToLLVM",
        "//third_party/llvm/llvm-project/mlir:VectorTransforms",
        "//third_party/llvm/llvm-project/mlir:X86VectorDialect",
        "//third_party/llvm/llvm-project/mlir:X86VectorTransforms",
        "//third_party/stablehlo:chlo_ops",
    ],
)

cc_library(
    name = "transforms_gpu_passes",
    srcs = [
        "transforms/gpu_kernel_lowering_passes.cc",
        "transforms/gpu_passes.cc",
        "transforms/gpu_passes.h.inc",
    ],
    hdrs = ["transforms/gpu_passes.h"],
    strip_include_prefix = ".",
    deps = [
        ":gpu_transforms_passes_inc_gen",
        ":mhlo_passes",
        ":mlir_hlo",
        ":transforms_passes",
        "//third_party/llvm/llvm-project/llvm:Support",
        "//third_party/llvm/llvm-project/mlir:AffineDialect",
        "//third_party/llvm/llvm-project/mlir:AffineToStandard",
        "//third_party/llvm/llvm-project/mlir:ArithDialect",
        "//third_party/llvm/llvm-project/mlir:ArithToLLVM",
        "//third_party/llvm/llvm-project/mlir:ArithTransforms",
        "//third_party/llvm/llvm-project/mlir:BufferizationDialect",
        "//third_party/llvm/llvm-project/mlir:BufferizationTransforms",
        "//third_party/llvm/llvm-project/mlir:ComplexToLLVM",
        "//third_party/llvm/llvm-project/mlir:ControlFlowToLLVM",
        "//third_party/llvm/llvm-project/mlir:FuncDialect",
        "//third_party/llvm/llvm-project/mlir:FuncToLLVM",
        "//third_party/llvm/llvm-project/mlir:GPUCommonTransforms",
        "//third_party/llvm/llvm-project/mlir:GPUDialect",
        "//third_party/llvm/llvm-project/mlir:GPUToGPURuntimeTransforms",
        "//third_party/llvm/llvm-project/mlir:GPUToNVVMTransforms",
        "//third_party/llvm/llvm-project/mlir:GPUToROCDLTransforms",
        "//third_party/llvm/llvm-project/mlir:GPUTransforms",
        "//third_party/llvm/llvm-project/mlir:IR",
        "//third_party/llvm/llvm-project/mlir:LLVMCommonConversion",
        "//third_party/llvm/llvm-project/mlir:LLVMDialect",
        "//third_party/llvm/llvm-project/mlir:LinalgTransforms",
        "//third_party/llvm/llvm-project/mlir:MathToLLVM",
        "//third_party/llvm/llvm-project/mlir:MemRefDialect",
        "//third_party/llvm/llvm-project/mlir:MemRefToLLVM",
        "//third_party/llvm/llvm-project/mlir:MemRefTransforms",
        "//third_party/llvm/llvm-project/mlir:NVVMDialect",
        "//third_party/llvm/llvm-project/mlir:Pass",
        "//third_party/llvm/llvm-project/mlir:ROCDLDialect",
        "//third_party/llvm/llvm-project/mlir:Rewrite",
        "//third_party/llvm/llvm-project/mlir:SCFToControlFlow",
        "//third_party/llvm/llvm-project/mlir:SCFTransforms",
        "//third_party/llvm/llvm-project/mlir:ShapeToStandard",
        "//third_party/llvm/llvm-project/mlir:TensorInferTypeOpInterfaceImpl",
        "//third_party/llvm/llvm-project/mlir:TransformUtils",
        "//third_party/llvm/llvm-project/mlir:Transforms",
        "//third_party/llvm/llvm-project/mlir:VectorDialect",
        "//third_party/llvm/llvm-project/mlir:VectorToLLVM",
        "//third_party/llvm/llvm-project/mlir:VectorTransforms",
    ],
)

gentbl_cc_library(
    name = "transforms_passes_inc_gen",
    compatible_with = get_compatible_with_portable(),
    strip_include_prefix = ".",
    tbl_outs = [
        (
            [
                "-gen-pass-decls",
                "-name=LMHLOTransforms",
            ],
            "transforms/passes.h.inc",
        ),
    ],
    tblgen = "//third_party/llvm/llvm-project/mlir:mlir-tblgen",
    td_file = "transforms/passes.td",
    deps = ["//third_party/llvm/llvm-project/mlir:PassBaseTdFiles"],
)

gentbl_cc_library(
    name = "gpu_transforms_passes_inc_gen",
    compatible_with = get_compatible_with_portable(),
    strip_include_prefix = ".",
    tbl_outs = [
        (
            [
                "-gen-pass-decls",
                "-name=LMHLOGPUTransforms",
            ],
            "transforms/gpu_passes.h.inc",
        ),
    ],
    tblgen = "//third_party/llvm/llvm-project/mlir:mlir-tblgen",
    td_file = "transforms/gpu_passes.td",
    deps = ["//third_party/llvm/llvm-project/mlir:PassBaseTdFiles"],
)

cc_library(
    name = "shape_component_analysis",
    srcs = ["mhlo/analysis/shape_component_analysis.cc"],
    hdrs = ["mhlo/analysis/shape_component_analysis.h"],
    strip_include_prefix = ".",
    deps = [
        ":mlir_hlo",
        "//third_party/llvm/llvm-project/llvm:Support",
        "//third_party/llvm/llvm-project/mlir:ArithDialect",
        "//third_party/llvm/llvm-project/mlir:FuncDialect",
        "//third_party/llvm/llvm-project/mlir:IR",
        "//third_party/llvm/llvm-project/mlir:ShapeDialect",
        "//third_party/llvm/llvm-project/mlir:Support",
        "//third_party/llvm/llvm-project/mlir:TensorDialect",
    ],
)

CAPI_HEADERS = [
    "bindings/c/Attributes.h",
    "bindings/c/Dialects.h",
    "bindings/c/Passes.h",
    "bindings/c/Types.h",
]

CAPI_SOURCES = [
    "bindings/c/Attributes.cc",
    "bindings/c/Dialects.cc",
    "bindings/c/Passes.cc",
    "bindings/c/Types.cc",
]

cc_library(
    name = "CAPI",
    srcs = CAPI_SOURCES,
    hdrs = CAPI_HEADERS,
    strip_include_prefix = ".",
    deps = [
        ":all_passes",
        ":mlir_hlo",
        "//third_party/llvm/llvm-project/mlir:CAPIIR",
        "//third_party/llvm/llvm-project/mlir:Support",
    ],
)

# Header-only target, used when using the C API from a separate shared library.
cc_library(
    name = "CAPIHeaders",
    hdrs = CAPI_HEADERS,
    strip_include_prefix = ".",
    deps = ["//third_party/llvm/llvm-project/mlir:CAPIIRHeaders"],
)

# Alwayslink target, used when exporting the C API from a shared library.
cc_library(
    name = "CAPIObjects",
    srcs = CAPI_SOURCES,
    hdrs = CAPI_HEADERS,
    strip_include_prefix = ".",
    deps = [
        ":all_passes",
        ":mlir_hlo",
        "//third_party/llvm/llvm-project/mlir:CAPIIRObjects",
        "//third_party/llvm/llvm-project/mlir:Support",
    ],
    alwayslink = True,
)

build_test(
    name = "mlir-hlo-opt_build_test",
    targets = [
        ":mlir-hlo-opt",
    ],
)

cc_binary(
    name = "mlir-hlo-opt",
    srcs = ["tools/mlir-hlo-opt/mlir-hlo-opt.cc"],
    deps = [
        ":all_passes",
        ":hlo_dialect_registration",
        ":stablehlo_extension_passes",
        ":transforms_gpu_passes",
        "//third_party/llvm/llvm-project/llvm:Support",
        "//third_party/llvm/llvm-project/mlir:AllExtensions",
        "//third_party/llvm/llvm-project/mlir:AllPassesAndDialects",
        "//third_party/llvm/llvm-project/mlir:IR",
        "//third_party/llvm/llvm-project/mlir:MlirOptLib",
        "//third_party/llvm/llvm-project/mlir:Pass",
        "//third_party/llvm/llvm-project/mlir:Support",
        "//third_party/stablehlo:register",
    ],
)

# Python library.

gentbl_filegroup(
    name = "MhloOpsPyGen",
    tbl_outs = [
        (
            [
                "-gen-python-op-bindings",
                "-bind-dialect=mhlo",
            ],
            "bindings/python/mlir/dialects/_mhlo_ops_gen.py",
        ),
    ],
    tblgen = "//third_party/llvm/llvm-project/mlir:mlir-tblgen",
    td_file = "bindings/python/mlir/dialects/MhloOps.td",
    deps = [
        ":hlo_ops_td_files",
        "//third_party/llvm/llvm-project/mlir:OpBaseTdFiles",
    ],
)

filegroup(
    name = "MhloOpsPyFiles",
    srcs = [
        "bindings/python/mlir/dialects/mhlo.py",
        ":MhloOpsPyGen",
    ],
)

# A light-weight runtime support library, used by MLIR code that results
# after lowering some ops in the vector and sparse tensor dialects.
cc_binary(
    name = "libmlir_c_runner_utils.so",
    linkshared = True,
    linkstatic = False,
    deps = ["//third_party/llvm/llvm-project/mlir:mlir_c_runner_utils"],
)

# StableHLO Extension

cc_library(
    name = "stablehlo_extension_base",
    srcs = [
        "stablehlo_ext/IR/base.cpp",
    ],
    hdrs = [
        "stablehlo_ext/IR/base.h",
    ],
    compatible_with = get_compatible_with_portable(),
    strip_include_prefix = ".",
    deps = [
        "//third_party/llvm/llvm-project/llvm:Support",
        "//third_party/llvm/llvm-project/mlir:IR",
    ],
)

cc_library(
    name = "stablehlo_extension_ops",
    srcs = [
        "stablehlo_ext/IR/stablehlo_ops.cpp",
    ],
    hdrs = [
        "stablehlo_ext/IR/stablehlo_ops.h",
    ],
    compatible_with = get_compatible_with_portable(),
    strip_include_prefix = ".",
    deps = [
        "//third_party/llvm/llvm-project/llvm:Support",
        "//third_party/llvm/llvm-project/mlir:FuncDialect",
        "//third_party/llvm/llvm-project/mlir:IR",
        "//third_party/llvm/llvm-project/mlir:Support",
        "//third_party/stablehlo:base",
        "//third_party/stablehlo:stablehlo_ops",
    ],
)

gentbl_cc_library(
    name = "stablehlo_extension_pass_inc_gen",
    compatible_with = get_compatible_with_portable(),
    strip_include_prefix = ".",
    tbl_outs = [
        (
            [
                "-gen-pass-decls",
            ],
            "stablehlo_ext/transforms/passes.h.inc",
        ),
    ],
    tblgen = "//third_party/llvm/llvm-project/mlir:mlir-tblgen",
    td_file = "stablehlo_ext/transforms/passes.td",
    deps = ["//third_party/llvm/llvm-project/mlir:PassBaseTdFiles"],
)

cc_library(
    name = "stablehlo_extension_passes",
    srcs = [
        "stablehlo_ext/transforms/chlo_recompose_ops.cpp",
        "stablehlo_ext/transforms/stablehlo_canonicalize_dynamism.cpp",
        "stablehlo_ext/transforms/stablehlo_refine_shapes.cpp",
    ],
    hdrs = [
        "stablehlo_ext/transforms/passes.h",
    ],
    compatible_with = get_compatible_with_portable(),
    strip_include_prefix = ".",
    deps = [
        ":stablehlo_extension_base",
        ":stablehlo_extension_ops",
        ":stablehlo_extension_pass_inc_gen",
        "//third_party/llvm/llvm-project/llvm:Support",
        "//third_party/llvm/llvm-project/mlir:FuncDialect",
        "//third_party/llvm/llvm-project/mlir:IR",
        "//third_party/llvm/llvm-project/mlir:InferTypeOpInterface",
        "//third_party/llvm/llvm-project/mlir:Pass",
        "//third_party/llvm/llvm-project/mlir:Support",
        "//third_party/llvm/llvm-project/mlir:TransformUtils",
        "//third_party/llvm/llvm-project/mlir:Transforms",
        "//third_party/stablehlo:base",
        "//third_party/stablehlo:chlo_ops",
        "//third_party/stablehlo:stablehlo_ops",
        "//third_party/stablehlo:stablehlo_ops_inc_gen",
        "//third_party/stablehlo:stablehlo_passes",
        "//third_party/stablehlo:stablehlo_type_inference",
    ],
)
