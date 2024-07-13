// RUN: xla-opt %s -sparse-blocked-to-mma | FileCheck %s

#blocked = #triton_gpu.blocked<{sizePerThread = [1, 1], threadsPerWarp = [8, 4], warpsPerCTA = [4, 1], order = [1, 0], CTAsPerCGA = [1, 1], CTASplitNum = [1, 1], CTAOrder = [1, 0]}>
// CHECK: #[[MMA:.+]] = #triton_gpu.nvidia_mma<{versionMajor = 2, versionMinor = 0, warpsPerCTA = [2, 2], instrShape = [16, 8]}>
#lhs = #triton_gpu.dot_op<{opIdx = 0, parent = #blocked}>
#rhs = #triton_gpu.dot_op<{opIdx = 1, parent = #blocked}>
module attributes {"triton_gpu.target" = "cuda:80", "triton_gpu.num-warps" = 4 : i32} {
  tt.func @sparse_dot(%A: tensor<64x32xf16, #lhs>, %B: tensor<64x64xf16, #rhs>, %meta: tensor<64x4xi16, #blocked>) -> tensor<64x64xf32, #blocked> {
    %C = arith.constant dense<0.000000e+00> : tensor<64x64xf32, #blocked>
    // CHECK-DAG: %[[LHS:.+]] = triton_gpu.convert_layout {{.+}} : {{.+}} -> tensor<64x32xf16, #triton_gpu.dot_op<{opIdx = 0, parent = #[[MMA]], kWidth = 2}>>
    // CHECK-DAG: %[[RHS:.+]] = triton_gpu.convert_layout {{.+}} : {{.+}} -> tensor<64x64xf16, #triton_gpu.dot_op<{opIdx = 1, parent = #[[MMA]], kWidth = 2}>>
    // CHECK-DAG: %[[ACC:.+]] = triton_gpu.convert_layout {{.+}} : {{.+}} -> tensor<64x64xf32, #[[MMA]]>
    // CHECK-DAG: %[[META:.+]] = triton_gpu.convert_layout {{.+}} : {{.+}} -> tensor<64x4xi16, #triton_gpu.sparse_dot_meta<{parent = #[[MMA]]}>>
    // CHECK: %[[OUT:.+]] = triton_gpu.sparse_dot %[[LHS]], %[[RHS]], %[[ACC]], %[[META]] : {{.+}} -> tensor<64x64xf32, #[[MMA]]>
    %D = triton_gpu.sparse_dot %A, %B, %C, %meta : tensor<64x32xf16, #lhs> meta tensor<64x4xi16, #blocked> * tensor<64x64xf16, #rhs> -> tensor<64x64xf32, #blocked>
    // CHECK: triton_gpu.convert_layout %[[OUT]] : tensor<64x64xf32, #[[MMA]]> -> tensor<64x64xf32, #blocked>
    tt.return %D : tensor<64x64xf32, #blocked>
  }
}