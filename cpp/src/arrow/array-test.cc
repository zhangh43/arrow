// Licensed to the Apache Software Foundation (ASF) under one
// or more contributor license agreements.  See the NOTICE file
// distributed with this work for additional information
// regarding copyright ownership.  The ASF licenses this file
// to you under the Apache License, Version 2.0 (the
// "License"); you may not use this file except in compliance
// with the License.  You may obtain a copy of the License at
//
//   http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing,
// software distributed under the License is distributed on an
// "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY
// KIND, either express or implied.  See the License for the
// specific language governing permissions and limitations
// under the License.

#include <cstdint>
#include <cstdlib>
#include <memory>
#include <vector>

#include "gtest/gtest.h"

#include "arrow/array.h"
#include "arrow/test-util.h"
#include "arrow/type.h"
#include "arrow/types/primitive.h"
#include "arrow/util/buffer.h"
#include "arrow/util/memory-pool.h"

namespace arrow {

class TestArray : public ::testing::Test {
 public:
  void SetUp() { pool_ = default_memory_pool(); }

 protected:
  MemoryPool* pool_;
};

TEST_F(TestArray, TestNullCount) {
  auto data = std::make_shared<PoolBuffer>(pool_);
  auto null_bitmap = std::make_shared<PoolBuffer>(pool_);

  std::unique_ptr<Int32Array> arr(new Int32Array(100, data, 10, null_bitmap));
  ASSERT_EQ(10, arr->null_count());

  std::unique_ptr<Int32Array> arr_no_nulls(new Int32Array(100, data));
  ASSERT_EQ(0, arr_no_nulls->null_count());
}

TEST_F(TestArray, TestLength) {
  auto data = std::make_shared<PoolBuffer>(pool_);
  std::unique_ptr<Int32Array> arr(new Int32Array(100, data));
  ASSERT_EQ(arr->length(), 100);
}

TEST_F(TestArray, TestIsNull) {
  // clang-format off
  std::vector<uint8_t> null_bitmap = {1, 0, 1, 1, 0, 1, 0, 0,
                                      1, 0, 1, 1, 0, 1, 0, 0,
                                      1, 0, 1, 1, 0, 1, 0, 0,
                                      1, 0, 1, 1, 0, 1, 0, 0,
                                      1, 0, 0, 1};
  // clang-format on
  int32_t null_count = 0;
  for (uint8_t x : null_bitmap) {
    if (x == 0) { ++null_count; }
  }

  std::shared_ptr<Buffer> null_buf = test::bytes_to_null_buffer(null_bitmap);
  std::unique_ptr<Array> arr;
  arr.reset(new Int32Array(null_bitmap.size(), nullptr, null_count, null_buf));

  ASSERT_EQ(null_count, arr->null_count());
  ASSERT_EQ(5, null_buf->size());

  ASSERT_TRUE(arr->null_bitmap()->Equals(*null_buf.get()));

  for (size_t i = 0; i < null_bitmap.size(); ++i) {
    EXPECT_EQ(null_bitmap[i], !arr->IsNull(i)) << i;
  }
}

TEST_F(TestArray, TestCopy) {}

}  // namespace arrow
