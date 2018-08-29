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

// See docs in ../ops/math_ops.cc.

#include "third_party/eigen3/unsupported/Eigen/CXX11/Tensor"
#include "tensorflow/core/framework/tensor_types.h"
#include "tensorflow/core/lib/gtl/inlined_vector.h"
#include "tensorflow/core/framework/kernel_def_builder.h"
#include "tensorflow/core/framework/op_kernel.h"
#include "tensorflow/core/framework/register_types.h"
#include "tensorflow/core/framework/tensor_shape.h"
#include "tensorflow/core/kernels/linalg_ops_common.h"
#include "tensorflow/core/lib/core/errors.h"
#include "tensorflow/core/platform/logging.h"
#include "tensorflow/core/platform/types.h"
#include "third_party/eigen3/Eigen/Core"
#include "third_party/eigen3/Eigen/LU"


namespace tensorflow {

typedef Eigen::ThreadPoolDevice CPUDevice;

static const char kErrMsg[] =
    "LU decomposition was not successful. The input might not be valid.";

template <typename Device, typename T>
class LuOp : public OpKernel {
 public:
  explicit LuOp(OpKernelConstruction* context) : OpKernel(context) {}
  
  void Compute(OpKernelContext* context) override {
    // TODO(hzhuang): Check the instability inside LU_op. 
    // return an integer containing the index of the first zero diagonal in U,
    // like INFO argument in LAPACK function xGETRF.    
    const Tensor & in = context->input(0);  
    TensorShape mtx_shape = in.shape();         
    // hzhuang: assume square at this moment        
    auto matrix = in.matrix<T>();
    
    auto & input = Eigen::Matrix<T, Eigen::Dynamic, Eigen::Dynamic, 
                          Eigen::RowMajor>::Map(
                            matrix.data(), matrix.dimension(0), matrix.dimension(1));

    Eigen::PartialPivLU<Eigen::Matrix<T, Eigen::Dynamic, 
                        Eigen::Dynamic, Eigen::RowMajor>> 
                          lu_decomposition(input);
            
    TensorShape perm_shape({});
    perm_shape.AddDim(mtx_shape.dim_size(0));

    // Create the output tensors
    Tensor * output_l = nullptr;
    Tensor * output_u = nullptr;
    Tensor * perm_idx = nullptr;
    Tensor * info_tensor = nullptr;
    
    OP_REQUIRES_OK(context, context->allocate_output(0, mtx_shape, &output_l));
    OP_REQUIRES_OK(context, context->allocate_output(1, mtx_shape, &output_u));
    OP_REQUIRES_OK(context, context->allocate_output(2, perm_shape, &perm_idx));
    OP_REQUIRES_OK(context, context->allocate_output(3, TensorShape({}), &info_tensor));
    
    T * ltensor = output_l->flat<T>().data();    
    auto &l = lu_decomposition.matrixLU().template triangularView<Eigen::UnitLower>();
    auto lm = Eigen::Map<Eigen::Matrix<T, Eigen::Dynamic, 
                Eigen::Dynamic, Eigen::RowMajor> >(ltensor, l.rows(), l.cols());
    lm = l;    
    T * utensor = output_u->flat<T>().data();
    auto &u = lu_decomposition.matrixLU().template triangularView<Eigen::Upper>();
    auto um = Eigen::Map<Eigen::Matrix<T, Eigen::Dynamic, 
                Eigen::Dynamic, Eigen::RowMajor> >(utensor, u.rows(), u.cols());
    // check the index of the first zero 
    /*
    info is a scalar integer 
          = 0:  successful exit
          < 0:  if info = -i, the i-th argument had an small value
          > 0:  if info = i, u(i,i) is exactly zero. 
    */
    double eps = 1e-9; // TODO (hzhuang): what is a good value
    int info = 0;
    for(int i = 0; i < u.rows(); i++){
      if(u(i,i) < eps && u(i,i) > -eps){
        info = -(i+1);
        break;
      }
      else if(u(i,i) == 0.){
        info = i+1;
        break;
      }
    }

    info_tensor->flat<int32>().setConstant(info);

    um = u;
    int32 * ptensor = perm_idx->flat<int32>().data();
    auto & p = lu_decomposition.permutationP().indices().array(); 
    
    for(int i = 0; i < p.size(); i++){
      ptensor[i] = p[i];
    }        

  }
};

#define REGISTER_KERNEL(T)                                               \
  REGISTER_KERNEL_BUILDER(                                               \
      Name("Lu").Device(DEVICE_CPU).TypeConstraint<T>("T"),              \
      LuOp<CPUDevice, T>)
 
REGISTER_KERNEL(float);
REGISTER_KERNEL(double);

#undef REGISTER_KERNEL
}  // namespace tensorflow
