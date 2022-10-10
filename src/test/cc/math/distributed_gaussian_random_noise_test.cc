// Copyright 2020 The Cross-Media Measurement Authors
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//      http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#include "math/distributed_gaussian_random_noise.h"

#include <unordered_map>

#include "absl/status/statusor.h"
#include "common_cpp/macros/macros.h"
#include "common_cpp/testing/status_macros.h"
#include "gtest/gtest.h"

namespace wfa::math {
namespace {
DistributedGaussianRandomNoise distributedGaussianRandomNoise;

TEST(GaussianRandomNoiseIndividualComponent, SimpleTest) {
  int64_t shift_offset = 20;
  int64_t truncate_threshold = 10;

  ASSERT_OK_AND_ASSIGN(int64_t result,
                       distributedGaussianRandomNoise.GenerateNoiseComponent(
                           {.num = 3,
                            .p = 0.6,
                            .truncate_threshold = truncate_threshold,
                            .shift_offset = shift_offset}));
  EXPECT_EQ(result, 100);
}
}  // namespace
}  // namespace wfa::math
