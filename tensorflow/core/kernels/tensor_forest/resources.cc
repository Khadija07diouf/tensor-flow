/* Copyright 2018 The TensorFlow Authors. All Rights Reserved.

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

#include "tensorflow/core/kernels/tensor_forest/resources.h"
#include "tensorflow/core/kernels/boosted_trees/boosted_trees.pb.h"

namespace tensorflow {

const float DecisionTreeResource::get_prediction(const int32 id,
                                                 const int32 dimension) const {
  return decision_tree_->nodes(id).leaf().vector().value(dimension);
};

const int32 DecisionTreeResource::TraverseTree(
    const std::unique_ptr<DenseTensorType>& input_data, int example_id) const {
  using boosted_trees::Node;
  using boosted_trees::Tree;
  int32 current_id = 0;
  while (true) {
    const Node& current = decision_tree_->nodes(current_id);
    if (current.has_leaf()) {
      return current_id;
    };
    DCHECK_EQ(current.node_case(), Node::kDenseSplit);
    const auto& split = current.dense_split();

    if ((*input_data)(example_id, split.feature_id()) <= split.threshold()) {
      current_id = split.left_id();
    } else {
      current_id = split.right_id();
    }
  }
};
}  // namespace tensorflow
