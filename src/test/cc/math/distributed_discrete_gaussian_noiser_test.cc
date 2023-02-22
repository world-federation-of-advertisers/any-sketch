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

#include "math/distributed_discrete_gaussian_noiser.h"

#include <unordered_map>

#include "common_cpp/macros/macros.h"
#include "common_cpp/testing/status_macros.h"
#include "common_cpp/testing/status_matchers.h"
#include "gtest/gtest.h"

namespace wfa::math {
namespace {

TEST(DiscreteGaussianNoiserIndividualComponent, StatusIsOK) {
  DistributedDiscreteGaussianNoiser distributed_discrete_gaussian_noiser(
      {.contributor_count = 1,
       .sigma = 6.87,
       .truncate_threshold = 13,
       .shift_offset = 13});

  ASSERT_THAT(distributed_discrete_gaussian_noiser.GenerateNoiseComponent(),
              IsOk());
}

TEST(DiscreteGaussianNoiserGenerateNoiseComponent, ReturnsSampledValues) {
  DistributedDiscreteGaussianNoiser distributed_discrete_gaussian_noiser(
      {.contributor_count = 1,
       .sigma = 6.87,
       .truncate_threshold = 13,
       .shift_offset = 13});
  int num = 5;
  std::vector<int64_t> results;

  for (size_t i = 0; i < num; ++i) {
    ASSERT_OK_AND_ASSIGN(
        int64_t temp,
        distributed_discrete_gaussian_noiser.GenerateNoiseComponent());
    results.push_back(temp);
  }

  ASSERT_THAT(results.size(), num);
}

TEST(DiscreteGaussianNoiserIndividualComponent, MinMaxShouldBeCorrect) {
  double sigma = 48.231;
  int64_t offset = 261;

  int64_t min_value = 1000;
  int64_t max_value = 0;

  size_t num_trials = 10000;

  DistributedDiscreteGaussianNoiser distributed_gaussian_noiser(
      {.contributor_count = 1,
       .sigma = sigma,
       .truncate_threshold = offset,
       .shift_offset = offset});

  for (size_t i = 0; i < num_trials; ++i) {
    ASSERT_OK_AND_ASSIGN(int64_t temp,
                         distributed_gaussian_noiser.GenerateNoiseComponent());

    min_value = std::min(min_value, temp);
    max_value = std::max(max_value, temp);
  }

  // Max should be smaller than 2 * shift_offset.
  ASSERT_LE(max_value, 2 * offset);
  // Min should be larger than 0.
  ASSERT_GE(min_value, 0);
}

TEST(DiscreteGaussianNoiserGlobalSummation,
     ProbabilityMassFunctionShouldBeCorrect) {
  int64_t contributor_count = 1;  // 1 contributor
  double sigma = 48.231;
  int64_t offset = 261;

  DistributedDiscreteGaussianNoiseComponentOptions options = {
      .contributor_count = contributor_count,
      .sigma = sigma,
      .truncate_threshold = offset,
      .shift_offset = offset};
  DistributedDiscreteGaussianNoiser distributed_discrete_gaussian_noiser(
      options);

  int64_t min_output = 0;
  int64_t max_output = 2 * offset;

  size_t num_trials = 10000;
  std::map<int64_t, size_t> frequency_distribution;

  for (size_t i = 0; i < num_trials; ++i) {
    ASSERT_OK_AND_ASSIGN(
        int64_t temp,
        distributed_discrete_gaussian_noiser.GenerateNoiseComponent());
    ASSERT_GE(temp, min_output);
    ASSERT_LE(temp, max_output);
    ++frequency_distribution[temp];
  }

  for (auto const& x : frequency_distribution) {
    double probability = static_cast<double>(x.second) / num_trials;
    // Expected probability is Gaussian(normal) distribution.
    double expected_probability =
        std::exp(-std::pow((x.first - offset), 2) / (2 * std::pow(sigma, 2))) /
        (sigma * std::sqrt(2 * M_PI));

    EXPECT_NEAR(probability, expected_probability, 0.01);
  }
}

}  // namespace
}  // namespace wfa::math
