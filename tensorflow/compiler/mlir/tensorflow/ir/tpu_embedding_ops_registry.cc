/* Copyright 2022 The TensorFlow Authors. All Rights Reserved.

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

#include "tensorflow/compiler/mlir/tensorflow/ir/tpu_embedding_ops_registry.h"

#include <vector>

#include "llvm/ADT/DenseSet.h"
#include "mlir/Support/TypeID.h"  // from @llvm-project

namespace mlir {
namespace TF {

const llvm::SmallDenseSet<mlir::TypeID>&
TPUEmbeddingOpsRegistry::GetOpsTypeIds() {
  return ops_type_ids_;
}

// static
TPUEmbeddingOpsRegistry& TPUEmbeddingOpsRegistry::Global() {
  static TPUEmbeddingOpsRegistry* registry = new TPUEmbeddingOpsRegistry;
  return *registry;
}
}  // namespace TF
}  // namespace mlir
