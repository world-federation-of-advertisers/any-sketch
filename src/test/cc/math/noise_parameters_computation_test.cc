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

#include "math/noise_parameters_computation.h"

#include "gtest/gtest.h"

namespace wfa::math {
namespace {

TEST(GetPublisherNoiseOptions, ExampleResultShouldBeCorrect) {
  wfa::any_sketch::DifferentialPrivacyParams test_params;
  test_params.set_epsilon(std::log(3) / 30);
  test_params.set_delta(0.2 / 100000);
  int publisher_count = 3;

  auto options = GetPublisherNoiseOptions(test_params, publisher_count);

  EXPECT_EQ(options.mu, 1222);
  EXPECT_NEAR(options.s, 0.0122, 0.0001);
}

}  // namespace
}  // namespace wfa::math
