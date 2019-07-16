/* Copyright 2015 The TensorFlow Authors. All Rights Reserved.

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

#include "tensorflow/core/framework/common_shape_fns.h"
#include "tensorflow/core/framework/numeric_op.h"
#include "tensorflow/core/framework/op.h"
#include "tensorflow/core/framework/shape_inference.h"

namespace tensorflow {


REGISTER_OP("CustomL2Norm")
    .Input("in: T")
    .Input("eps: float32")
    .Input("scale: float32")
    .Input("bias: float32")
    .Output("out: T")
    .Attr("T: {float16,float32}")
    .SetShapeFn([](::tensorflow::shape_inference::InferenceContext* c) {
      c->set_output(0, c->input(0));
      return Status::OK();
    });

REGISTER_OP("CustomL2NormGrad")
    .Input("in: T")
    .Input("eps: float32")
    .Input("scale: float32")
    .Input("bias: float32")
    .Input("outgrad: T")
    .Output("out: T")
    .Attr("T: {float16,float32}")
    .SetShapeFn([](::tensorflow::shape_inference::InferenceContext* c) {
      c->set_output(0, c->input(0));
      return Status::OK();
    });

}  // namespace tensorflow
