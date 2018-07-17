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

#include <memory>
#include <string>
#include <vector>

#include "tensorflow/compiler/xla/layout_util.h"
#include "tensorflow/compiler/xla/literal.h"
#include "tensorflow/compiler/xla/service/device_memory_allocator.h"
#include "tensorflow/compiler/xla/service/generic_transfer_manager.h"
#include "tensorflow/compiler/xla/service/shaped_buffer.h"
#include "tensorflow/compiler/xla/shape_util.h"
#include "tensorflow/compiler/xla/statusor.h"
#include "tensorflow/compiler/xla/tests/literal_test_util.h"
#include "tensorflow/compiler/xla/tests/local_client_test_base.h"
#include "tensorflow/compiler/xla/tests/test_macros.h"
#include "tensorflow/compiler/xla/types.h"
#include "tensorflow/compiler/xla/xla_data.pb.h"
#include "tensorflow/core/platform/logging.h"
#include "tensorflow/core/platform/stream_executor_no_cuda.h"
#include "tensorflow/core/platform/test_benchmark.h"
#include "tensorflow/core/platform/types.h"

namespace xla {
namespace {

class TransferManagerTest : public LocalClientTestBase {
 protected:
  TransferManagerTest()
      : shape_size_fn_([this](const Shape& shape) {
          return transfer_manager_->GetByteSizeRequirement(shape);
        }) {
    stream_ptr_ = local_client_->mutable_backend()
                      ->BorrowStream(stream_executor_)
                      .ValueOrDie();
    stream_ = stream_ptr_.get();
  }

  ~TransferManagerTest() override = default;

  ScopedShapedBuffer AllocateDeviceBuffer(const Shape& shape) {
    return transfer_manager_
        ->AllocateScopedShapedBuffer(
            shape, GetOrCreateAllocator(local_client_->platform()),
            /*device_ordinal=*/0)
        .ValueOrDie();
  }

 protected:
  Backend::StreamPtr stream_ptr_;
  se::Stream* stream_;

 private:
  std::function<int64(const Shape&)> shape_size_fn_;
};

XLA_TEST_F(TransferManagerTest, TransferR0U32) {
  std::unique_ptr<Literal> literal = LiteralUtil::CreateR0<uint32>(42);
  const Shape& shape = literal->shape();
  auto device_buffer = AllocateDeviceBuffer(shape);

  // Round trip literal through device.
  ASSERT_IS_OK(transfer_manager_->TransferLiteralToDevice(stream_, *literal,
                                                          device_buffer));
  TF_ASSERT_OK_AND_ASSIGN(
      std::unique_ptr<Literal> result,
      transfer_manager_->TransferLiteralFromDevice(stream_, device_buffer));

  LiteralTestUtil::ExpectR0Equal<uint32>(42, *result);
}

XLA_TEST_F(TransferManagerTest, TransferR1F32) {
  std::unique_ptr<Literal> literal =
      LiteralUtil::CreateR1<float>({1.25f, 2.5f, -17.0f, -20.125f});
  const Shape& shape = literal->shape();
  auto device_buffer = AllocateDeviceBuffer(shape);

  // Round trip literal through device.
  ASSERT_IS_OK(transfer_manager_->TransferLiteralToDevice(stream_, *literal,
                                                          device_buffer));
  TF_ASSERT_OK_AND_ASSIGN(
      std::unique_ptr<Literal> result,
      transfer_manager_->TransferLiteralFromDevice(stream_, device_buffer));

  LiteralTestUtil::ExpectR1Equal<float>({1.25f, 2.5f, -17.0f, -20.125f},
                                        *result);
}

XLA_TEST_F(TransferManagerTest, TransferR1LargeF32) {
  std::vector<float> test_vector(1024 * 1024);
  std::iota(test_vector.begin(), test_vector.end(), 0);
  std::unique_ptr<Literal> literal = LiteralUtil::CreateR1<float>(test_vector);
  const Shape& shape = literal->shape();
  auto device_buffer = AllocateDeviceBuffer(shape);

  // Round trip literal through device.
  ASSERT_IS_OK(transfer_manager_->TransferLiteralToDevice(stream_, *literal,
                                                          device_buffer));
  TF_ASSERT_OK_AND_ASSIGN(
      std::unique_ptr<Literal> result,
      transfer_manager_->TransferLiteralFromDevice(stream_, device_buffer));

  LiteralTestUtil::ExpectR1Equal<float>(test_vector, *result);
}

XLA_TEST_F(TransferManagerTest, TransferR1U8) {
  const char* test_string = "0123456789abcdef";
  std::unique_ptr<Literal> literal = LiteralUtil::CreateR1U8(test_string);
  const Shape& shape = literal->shape();
  auto device_buffer = AllocateDeviceBuffer(shape);

  // Round trip literal through device.
  ASSERT_IS_OK(transfer_manager_->TransferLiteralToDevice(stream_, *literal,
                                                          device_buffer));
  TF_ASSERT_OK_AND_ASSIGN(
      std::unique_ptr<Literal> result,
      transfer_manager_->TransferLiteralFromDevice(stream_, device_buffer));

  EXPECT_EQ(result->GetR1U8AsString(), test_string);
}

XLA_TEST_F(TransferManagerTest, TransferR2F32) {
  std::unique_ptr<Literal> literal =
      LiteralUtil::CreateR2<float>({{1.0f, 2.0f, 3.0f}, {4.0f, 5.0f, 6.0f}});
  const Shape& shape = literal->shape();
  auto device_buffer = AllocateDeviceBuffer(shape);

  // Round trip literal through device.
  ASSERT_IS_OK(transfer_manager_->TransferLiteralToDevice(stream_, *literal,
                                                          device_buffer));
  TF_ASSERT_OK_AND_ASSIGN(
      std::unique_ptr<Literal> result,
      transfer_manager_->TransferLiteralFromDevice(stream_, device_buffer));

  LiteralTestUtil::ExpectR2Equal<float>(
      {{1.0f, 2.0f, 3.0f}, {4.0f, 5.0f, 6.0f}}, *result);
}

XLA_TEST_F(TransferManagerTest,
           TransferR2F32AndChangeLayoutTransferringToDevice) {
  std::unique_ptr<Literal> literal = LiteralUtil::CreateR2WithLayout<float>(
      {{1.0f, 2.0f, 3.0f}, {4.0f, 5.0f, 6.0f}}, LayoutUtil::MakeLayout({0, 1}));
  const Shape ondevice_shape =
      ShapeUtil::MakeShapeWithLayout(F32, {2, 3}, {1, 0});
  auto device_buffer = AllocateDeviceBuffer(ondevice_shape);

  // Round trip literal through device. Set the on-device layout to something
  // different than the literal layout.
  ASSERT_IS_OK(transfer_manager_->TransferLiteralToDevice(stream_, *literal,
                                                          device_buffer));
  TF_ASSERT_OK_AND_ASSIGN(
      std::unique_ptr<Literal> result,
      transfer_manager_->TransferLiteralFromDevice(stream_, device_buffer));

  EXPECT_FALSE(
      LayoutUtil::Equal(result->shape().layout(), literal->shape().layout()));
  LiteralTestUtil::ExpectR2Equal<float>(
      {{1.0f, 2.0f, 3.0f}, {4.0f, 5.0f, 6.0f}}, *result);
}

XLA_TEST_F(TransferManagerTest, TransferTuple) {
  std::unique_ptr<Literal> literal = LiteralUtil::MakeTuple(
      {LiteralUtil::CreateR0<float>(123.0f).get(),
       LiteralUtil::CreateR2<float>({{1.0f, 2.0f}, {4.0f, 5.0f}}).get(),
       LiteralUtil::CreateR1<float>({44.0f, -10.0f, 3333333.3f}).get()});
  auto device_buffer = AllocateDeviceBuffer(literal->shape());

  // Round trip literal through device.
  ASSERT_IS_OK(transfer_manager_->TransferLiteralToDevice(stream_, *literal,
                                                          device_buffer));
  TF_ASSERT_OK_AND_ASSIGN(
      std::unique_ptr<Literal> result,
      transfer_manager_->TransferLiteralFromDevice(stream_, device_buffer));

  EXPECT_TRUE(LiteralTestUtil::Equal(*literal, *result));
}

XLA_TEST_F(TransferManagerTest, TransferEmptyTuple) {
  std::unique_ptr<Literal> literal = LiteralUtil::MakeTuple({});
  auto device_buffer = AllocateDeviceBuffer(literal->shape());

  // Round trip literal through device.
  ASSERT_IS_OK(transfer_manager_->TransferLiteralToDevice(stream_, *literal,
                                                          device_buffer));
  TF_ASSERT_OK_AND_ASSIGN(
      std::unique_ptr<Literal> result,
      transfer_manager_->TransferLiteralFromDevice(stream_, device_buffer));

  EXPECT_TRUE(LiteralTestUtil::Equal(*literal, *result));
}

XLA_TEST_F(TransferManagerTest, TransferNestedTuple) {
  std::unique_ptr<Literal> literal = LiteralUtil::MakeTuple(
      {LiteralUtil::CreateR0<float>(123.0f).get(),
       LiteralUtil::MakeTuple(
           {LiteralUtil::CreateR2<float>({{1.0f, 2.0f}, {4.0f, 5.0f}}).get(),
            LiteralUtil::CreateR1<float>({44.0f, -10.0f, 3333333.3f}).get()})
           .get(),
       LiteralUtil::CreateR1<float>({-10.0f, 123.0f}).get()});
  auto device_buffer = AllocateDeviceBuffer(literal->shape());

  // Round trip literal through device.
  ASSERT_IS_OK(transfer_manager_->TransferLiteralToDevice(stream_, *literal,
                                                          device_buffer));
  TF_ASSERT_OK_AND_ASSIGN(
      std::unique_ptr<Literal> result,
      transfer_manager_->TransferLiteralFromDevice(stream_, device_buffer));

  EXPECT_TRUE(LiteralTestUtil::Equal(*literal, *result));
}

XLA_TEST_F(TransferManagerTest, TransferComplexValue) {
  std::unique_ptr<Literal> literal = LiteralUtil::CreateR1<complex64>(
      {complex64(1.0f, 2.0f), complex64(42.0f, -123.4f)});
  auto device_buffer = AllocateDeviceBuffer(literal->shape());

  // Round trip literal through device.
  ASSERT_IS_OK(transfer_manager_->TransferLiteralToDevice(stream_, *literal,
                                                          device_buffer));
  TF_ASSERT_OK_AND_ASSIGN(
      std::unique_ptr<Literal> result,
      transfer_manager_->TransferLiteralFromDevice(stream_, device_buffer));

  EXPECT_TRUE(LiteralTestUtil::Equal(*literal, *result));
}

XLA_TEST_F(TransferManagerTest, TransferComplexValueInTuple) {
  std::unique_ptr<Literal> literal = LiteralUtil::MakeTuple(
      {LiteralUtil::CreateR1<complex64>(
           {complex64(1.0f, 2.0f), complex64(42.0f, -123.4f)})
           .get(),
       LiteralUtil::CreateR1<int32>({1, 2, 3, 4, 5, 6}).get(),
       LiteralUtil::CreateR0<complex64>(complex64(0.3f, -0.4f)).get()});
  auto device_buffer = AllocateDeviceBuffer(literal->shape());

  // Round trip literal through device.
  ASSERT_IS_OK(transfer_manager_->TransferLiteralToDevice(stream_, *literal,
                                                          device_buffer));
  TF_ASSERT_OK_AND_ASSIGN(
      std::unique_ptr<Literal> result,
      transfer_manager_->TransferLiteralFromDevice(stream_, device_buffer));

  EXPECT_TRUE(LiteralTestUtil::Equal(*literal, *result));
}

XLA_TEST_F(TransferManagerTest, TransferTokenFromDevice) {
  // "Copy" a token from the device. The token has no physical representation so
  // no copying is actually performed, but it shouldn't fail.
  // TODO(b/110532604): Add transferring the token to device when this is
  // supported.
  auto device_buffer = AllocateDeviceBuffer(ShapeUtil::MakeTokenShape());
  TF_ASSERT_OK_AND_ASSIGN(
      std::unique_ptr<Literal> result,
      transfer_manager_->TransferLiteralFromDevice(stream_, device_buffer));
  EXPECT_TRUE(LiteralTestUtil::Equal(*LiteralUtil::CreateToken(), *result));
}

XLA_TEST_F(TransferManagerTest, MultiStreamRoundTripSoak) {
  const int64 kIterationCount = 5000;
  std::unique_ptr<Literal> literal1 = LiteralUtil::MakeTuple(
      {LiteralUtil::CreateR0<float>(123.0f).get(),
       LiteralUtil::MakeTuple(
           {LiteralUtil::CreateR2<float>({{1.0f, 2.0f}, {4.0f, 5.0f}}).get(),
            LiteralUtil::CreateR1<float>({44.0f, -10.0f, 3333333.3f}).get()})
           .get(),
       LiteralUtil::CreateR1<float>({-10.0f, 123.0f}).get()});
  std::unique_ptr<Literal> literal2 = LiteralUtil::MakeTuple(
      {LiteralUtil::CreateR0<float>(456.0f).get(),
       LiteralUtil::MakeTuple(
           {LiteralUtil::CreateR2<float>({{5.0f, 7.0f}, {9.0f, 4.0f}}).get(),
            LiteralUtil::CreateR1<float>({44.0f, -11.0f, 3333333.3f}).get()})
           .get(),
       LiteralUtil::CreateR1<float>({-98.0f, 153.0f}).get()});

  auto device_buffer1 = AllocateDeviceBuffer(literal1->shape());
  auto device_buffer2 = AllocateDeviceBuffer(literal2->shape());

  auto stream1 = stream_;
  auto stream2 = stream_->GetOrCreateSubStream();

  std::unique_ptr<Literal> result1, result2;

  // Round trip literals through device in multiple streams asynchronously.
  for (int i = 0; i < kIterationCount; ++i) {
    ASSERT_IS_OK(transfer_manager_->TransferLiteralToDevice(stream1, *literal1,
                                                            device_buffer1));
    ASSERT_IS_OK(transfer_manager_->TransferLiteralToDevice(stream2, *literal2,
                                                            device_buffer2));
    TF_ASSERT_OK_AND_ASSIGN(
        std::unique_ptr<Literal> this_result1,
        transfer_manager_->TransferLiteralFromDevice(stream1, device_buffer1));
    TF_ASSERT_OK_AND_ASSIGN(
        std::unique_ptr<Literal> this_result2,
        transfer_manager_->TransferLiteralFromDevice(stream2, device_buffer2));
    result1 = std::move(this_result1);
    result2 = std::move(this_result2);
  }

  EXPECT_TRUE(LiteralTestUtil::Equal(*literal1, *result1));
  EXPECT_TRUE(LiteralTestUtil::Equal(*literal2, *result2));
}

class TransferDeviceToHostBenchmark : public TransferManagerTest {
 public:
  using TransferManagerTest::TransferManagerTest;
  ~TransferDeviceToHostBenchmark() override {}

  void Run(int iters, int num_tuple_elements, int array_size) {
    tensorflow::testing::StopTiming();
    SetUp();

    std::vector<std::unique_ptr<Literal>> tuple_elements;
    for (int i = 0; i < num_tuple_elements; ++i) {
      tuple_elements.push_back(
          LiteralUtil::CreateR2F32Linspace(0.0f, 1.0f, array_size, array_size));
    }
    std::unique_ptr<Literal> literal =
        LiteralUtil::MakeTupleOwned(std::move(tuple_elements));
    auto device_buffer = AllocateDeviceBuffer(literal->shape());
    TF_CHECK_OK(transfer_manager_->TransferLiteralToDevice(stream_, *literal,
                                                           device_buffer));
    tensorflow::testing::StartTiming();
    for (int i = 0; i < iters; ++i) {
      TF_ASSERT_OK_AND_ASSIGN(
          std::unique_ptr<Literal> result,
          transfer_manager_->TransferLiteralFromDevice(stream_, device_buffer));
    }
    tensorflow::testing::StopTiming();
    TearDown();
  }

  void TestBody() override {}
};

class TransferHostToDeviceBenchmark : public TransferManagerTest {
 public:
  using TransferManagerTest::TransferManagerTest;
  ~TransferHostToDeviceBenchmark() override {}

  void Run(int iters, int num_tuple_elements, int array_size) {
    tensorflow::testing::StopTiming();
    SetUp();

    std::vector<std::unique_ptr<Literal>> tuple_elements;
    for (int i = 0; i < num_tuple_elements; ++i) {
      tuple_elements.push_back(
          LiteralUtil::CreateR2F32Linspace(0.0f, 1.0f, array_size, array_size));
    }
    std::unique_ptr<Literal> literal =
        LiteralUtil::MakeTupleOwned(std::move(tuple_elements));
    auto device_buffer = AllocateDeviceBuffer(literal->shape());
    tensorflow::testing::StartTiming();
    for (int i = 0; i < iters; ++i) {
      TF_CHECK_OK(transfer_manager_->TransferLiteralToDevice(stream_, *literal,
                                                             device_buffer));
    }
    tensorflow::testing::StopTiming();
    TearDown();
  }

  void TestBody() override {}
};

void BM_TransferDeviceToHost(int iters, int num_tuple_elements,
                             int array_size) {
  TransferDeviceToHostBenchmark bm;
  bm.Run(iters, num_tuple_elements, array_size);
}

void BM_TransferHostToDevice(int iters, int num_tuple_elements,
                             int array_size) {
  TransferHostToDeviceBenchmark bm;
  bm.Run(iters, num_tuple_elements, array_size);
}

BENCHMARK(BM_TransferHostToDevice)
    ->ArgPair(1, 256)
    ->ArgPair(1, 257)
    ->ArgPair(100, 256)
    ->ArgPair(100, 257);

BENCHMARK(BM_TransferDeviceToHost)
    ->ArgPair(1, 256)
    ->ArgPair(1, 257)
    ->ArgPair(100, 256)
    ->ArgPair(100, 257);

int main(int argc, char** argv) {
  ::testing::InitGoogleTest(&argc, argv);
  tensorflow::testing::RunBenchmarks();
  return RUN_ALL_TESTS();
}

}  // namespace
}  // namespace xla
