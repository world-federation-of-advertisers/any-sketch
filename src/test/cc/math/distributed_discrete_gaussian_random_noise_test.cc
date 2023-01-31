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

#include "math/distributed_discrete_gaussian_random_noise.h"

#include <unordered_map>

#include "common_cpp/macros/macros.h"
#include "common_cpp/testing/status_macros.h"
#include "common_cpp/testing/status_matchers.h"
#include "gtest/gtest.h"

namespace wfa::math {
namespace {
DistributedDiscreteGaussianRandomNoise distributedGaussianRandomNoise(0.73);

TEST(GaussianRandomNoiseIndividualComponent, StatusIsOK) {
  ASSERT_THAT(distributedGaussianRandomNoise.GenerateNoiseComponent(), IsOk());
}

TEST(GaussianRandomNoiseGenerateNoiseComponent, ReturnsSampledValues) {
  int num = 5;
  std::vector<int64_t> results;

  for (size_t i = 0; i < num; ++i) {
    ASSERT_OK_AND_ASSIGN(
        int64_t temp, distributedGaussianRandomNoise.GenerateNoiseComponent());
    results.push_back(temp);
  }

  ASSERT_THAT(results.size(), num);
}

}  // namespace
}  // namespace wfa::math
