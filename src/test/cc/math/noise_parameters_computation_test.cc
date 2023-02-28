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

TEST(GetGeometricPublisherNoiseOptions, ExampleResultShouldBeCorrect) {
  wfa::any_sketch::DifferentialPrivacyParams test_params;
  test_params.set_epsilon(std::log(3) / 10);
  test_params.set_delta(0.2 / 100000);
  int publisher_count = 3;

  auto options =
      GetGeometricPublisherNoiseOptions(test_params, publisher_count);

  EXPECT_EQ(options.contributor_count, 1);
  EXPECT_NEAR(options.p, 0.964, 0.001);
  EXPECT_EQ(options.shift_offset, 428);
  EXPECT_EQ(options.truncate_threshold, 428);
}

TEST(GetDiscreteGaussianPublisherNoiseOptions, ExampleResultShouldBeCorrect) {
  wfa::any_sketch::DifferentialPrivacyParams test_params;
  test_params.set_epsilon(std::log(3) / 10);
  test_params.set_delta(0.2 / 100000);
  int64_t contributor_count = 1;

  auto options =
      GetDiscreteGaussianPublisherNoiseOptions(test_params, contributor_count);

  EXPECT_NEAR(options.sigma_distributed, 48.231, 0.001);
  EXPECT_EQ(options.shift_offset, 261);
  EXPECT_EQ(options.truncate_threshold, 261);
}

TEST(GetDiscreteGaussianPublisherNoiseOptions,
     ExampleResultShouldBeScaledByContributorCount) {
  wfa::any_sketch::DifferentialPrivacyParams test_params;
  test_params.set_epsilon(std::log(3) / 10);
  test_params.set_delta(0.2 / 100000);
  int64_t contributor_count = 4;

  auto options =
      GetDiscreteGaussianPublisherNoiseOptions(test_params, contributor_count);
  // sigma_distributed = sigma / sqrt(contributor_count)
  EXPECT_NEAR(options.sigma_distributed, 24.115, 0.001);
  // mu should be scaled by contributor_count.
  EXPECT_EQ(options.shift_offset, 137);
  EXPECT_EQ(options.truncate_threshold, 137);
}

}  // namespace
}  // namespace wfa::math
