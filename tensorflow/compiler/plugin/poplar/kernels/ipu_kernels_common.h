/* Copyright 2017 Graphcore Ltd
 */

/* Copyright 2017 The TensorFlow Authors. All Rights Reserved.

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.
==============================================================================*/

#ifndef TENSORFLOW_COMPILER_PLUGIN_POPLAR_KERNELS_IPU_KERNELS_COMMON_H_
#define TENSORFLOW_COMPILER_PLUGIN_POPLAR_KERNELS_IPU_KERNELS_COMMON_H_

#include "absl/container/flat_hash_set.h"
#include "tensorflow/compiler/plugin/poplar/kernels/custom_kernels_util.h"
#include "tensorflow/core/platform/types.h"

#define REGISTER_IPU_OP(OP_NAME, IMPL)                                     \
  REGISTER_KERNEL_BUILDER(Name(OP_NAME).Device(DEVICE_IPU_XLA_JIT), IMPL); \
  REGISTER_KERNEL_BUILDER(Name(OP_NAME).Device(DEVICE_XLA_IPU), IMPL);

namespace tensorflow {
// A class used to make sure that kernels set the properties expected by all
// custom ops.
class IpuOpKernel {
 protected:
  IpuOpKernel();

  // Allocating indexes used by the Allocation Finder - op specific.
  virtual const absl::flat_hash_set<int64> AllocatingIndexes() = 0;

  // Return how many of the first n operands are updated in place. If 0, the op
  // is treated as NotInplace.
  virtual const uint64 NumberOfInplaceOperands() = 0;

  // Adds all the required attributes to the map.
  void AddRequiredAttributesToMap();

  xla::poplarplugin::IPUCustomKernelsUtil::AttributeMap attribute_map_;
};
}  // namespace tensorflow

#endif  // TENSORFLOW_COMPILER_PLUGIN_POPLAR_KERNELS_IPU_KERNELS_COMMON_H_
