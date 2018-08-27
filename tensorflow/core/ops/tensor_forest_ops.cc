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

#include <vector>

#include "tensorflow/core/framework/common_shape_fns.h"
#include "tensorflow/core/framework/op.h"
#include "tensorflow/core/framework/resource_mgr.h"
#include "tensorflow/core/framework/shape_inference.h"

namespace tensorflow {

REGISTER_RESOURCE_HANDLE_OP(DecisionTreeResource);

REGISTER_OP("TensorForestTreeIsInitializedOp")
    .Input("tree_handle: resource")
    .Output("is_initialized: bool")
    .SetShapeFn(tensorflow::shape_inference::ScalarShape);

REGISTER_OP("TensorForestCreateTreeVariable")
    .Input("tree_handle: resource")
    .Input("tree_config: string")
    .SetShapeFn(tensorflow::shape_inference::NoOutputs);

REGISTER_OP("TensorForestTreeSerialize")
    .Input("tree_handle: resource")
    .Output("tree_config: string")
    .SetShapeFn(tensorflow::shape_inference::ScalarShape);

REGISTER_OP("TensorForestTreeDeserialize")
    .Input("tree_handle: resource")
    .Input("tree_config: string")
    .SetShapeFn(tensorflow::shape_inference::NoOutputs);

REGISTER_OP("TensorForestTreeSize")
    .Input("tree_handle: resource")
    .Output("tree_size: int32")
    .SetShapeFn(tensorflow::shape_inference::ScalarShape);

REGISTER_OP("TensorForestTreePredict")
    .Attr("logits_dimension: int")
    .Input("tree_handle: resource")
    .Input("dense_features: float")
    .Output("logits: float")
    .SetShapeFn([](tensorflow::shape_inference::InferenceContext* c) {
      shape_inference::DimensionHandle batch_size = c->UnknownDim();

      if (c->RankKnown(c->input(1)) && c->Rank(c->input(1)) > 0 &&
          c->Value(c->Dim(c->input(1), 0)) > 0) {
        batch_size = c->Dim(c->input(1), 0);
      }

      int logits_dimension;
      TF_RETURN_IF_ERROR(c->GetAttr("logits_dimension", &logits_dimension));
      c->set_output(0, c->Matrix(batch_size, logits_dimension));
      return Status::OK();
    });
}  // namespace tensorflow
