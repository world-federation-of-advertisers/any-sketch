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
constexpr int kContributorCount = 1;
constexpr int kOffset = 10;
constexpr int kSigmaDistributed = 5;

std::map<int64_t, double> GetExpectedProbabilityDistribution(
    int64_t min_output, int64_t max_output, int64_t offset,
    double sigma_distributed) {
  std::map<int64_t, double> expected_probability_distribution;
  double sum = 0.0;

  for (int64_t x = min_output; x <= max_output; ++x) {
    double expected_pmf_value = std::exp(-std::pow((x - offset), 2) /
                                         (2 * std::pow(sigma_distributed, 2)));
    expected_probability_distribution[x] = expected_pmf_value;
    sum += expected_pmf_value;
  }

  for (int64_t x = min_output; x <= max_output; ++x) {
    expected_probability_distribution[x] /= sum;
  }

  return expected_probability_distribution;
}

TEST(DiscreteGaussianNoiserGenerateNoiseComponent, StatusIsOK) {
  DistributedDiscreteGaussianNoiser distributed_discrete_gaussian_noiser(
      DistributedDiscreteGaussianNoiseComponentOptions{
          kContributorCount, kSigmaDistributed, kOffset, kOffset});

  ASSERT_THAT(distributed_discrete_gaussian_noiser.GenerateNoiseComponent(),
              IsOk());
}

TEST(DiscreteGaussianNoiserGenerateNoiseComponent,
     TruncatedThresholdIsNegativeOneStatusIsOK) {
  DistributedDiscreteGaussianNoiser distributed_discrete_gaussian_noiser(
      DistributedDiscreteGaussianNoiseComponentOptions{
          kContributorCount, kSigmaDistributed, kOffset});

  ASSERT_THAT(distributed_discrete_gaussian_noiser.GenerateNoiseComponent(),
              IsOk());
}

TEST(DiscreteGaussianNoiserGenerateNoiseComponent, ReturnsSampledValues) {
  DistributedDiscreteGaussianNoiser distributed_discrete_gaussian_noiser(
      DistributedDiscreteGaussianNoiseComponentOptions{
          kContributorCount, kSigmaDistributed, kOffset, kOffset});
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

TEST(DiscreteGaussianNoiserGenerateNoiseComponentRandomSamples,
     MinMaxShouldBeCorrect) {
  double sigma_distributed = kSigmaDistributed;
  int64_t offset = kOffset;

  int64_t min_value = 1000;
  int64_t max_value = 0;

  size_t num_trials = 10000;

  DistributedDiscreteGaussianNoiser distributed_gaussian_noiser(
      DistributedDiscreteGaussianNoiseComponentOptions{
          kContributorCount, sigma_distributed, offset, offset});

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

TEST(DiscreteGaussianNoiserGenerateNoiseComponentRandomSamples,
     ProbabilityMassFunctionShouldBeCorrectWithOneContributor) {
  int64_t contributor_count = 1;  // 1 contributor
  double sigma_distributed = kSigmaDistributed;
  int64_t offset = kOffset;

  DistributedDiscreteGaussianNoiseComponentOptions options{
      contributor_count, sigma_distributed, offset, offset};
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

  std::map<int64_t, double> expected_probability_distribution =
      GetExpectedProbabilityDistribution(min_output, max_output, offset,
                                         sigma_distributed);

  for (int64_t x = min_output; x <= max_output; ++x) {
    double probability =
        static_cast<double>(frequency_distribution[x]) / num_trials;

    EXPECT_NEAR(probability, expected_probability_distribution[x], 0.01);
  }
}

TEST(DiscreteGaussianNoiserGenerateNoiseComponentRandomSamples,
     ProbabilityMassFunctionShouldBeCorrectWithFourContributor) {
  int64_t contributor_count = 4;  // 4 contributors
  double sigma = 20;
  double sigma_distributed = sigma / std::sqrt(contributor_count);
  int64_t offset = kOffset;

  DistributedDiscreteGaussianNoiseComponentOptions options{
      contributor_count, sigma_distributed, offset, offset};
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

  std::map<int64_t, double> expected_probability_distribution =
      GetExpectedProbabilityDistribution(min_output, max_output, offset,
                                         sigma_distributed);

  for (int64_t x = min_output; x <= max_output; ++x) {
    double probability =
        static_cast<double>(frequency_distribution[x]) / num_trials;

    EXPECT_NEAR(probability, expected_probability_distribution[x], 0.01);
  }
}

TEST(DiscreteGaussianNoiserGenerateNoiseComponentRandomSamples,
     ProbabilityMassFunctionShouldBeCorrectWithDifferentOffset) {
  int64_t contributor_count = kContributorCount;
  double sigma_distributed = kSigmaDistributed;
  int64_t offset = 20;

  DistributedDiscreteGaussianNoiseComponentOptions options{
      contributor_count, sigma_distributed, offset, offset};
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

  std::map<int64_t, double> expected_probability_distribution =
      GetExpectedProbabilityDistribution(min_output, max_output, offset,
                                         sigma_distributed);

  for (int64_t x = min_output; x <= max_output; ++x) {
    double probability =
        static_cast<double>(frequency_distribution[x]) / num_trials;

    EXPECT_NEAR(probability, expected_probability_distribution[x], 0.01);
  }
}

TEST(DiscreteGaussianNoiserGenerateNoiseComponentRandomSamples,
     ProbabilityMassFunctionShouldBeCorrectWithDifferentSigma) {
  int64_t contributor_count = kContributorCount;
  double sigma_distributed = 1;
  int64_t offset = kOffset;

  DistributedDiscreteGaussianNoiseComponentOptions options{
      contributor_count, sigma_distributed, offset, offset};
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

  std::map<int64_t, double> expected_probability_distribution =
      GetExpectedProbabilityDistribution(min_output, max_output, offset,
                                         sigma_distributed);

  for (int64_t x = min_output; x <= max_output; ++x) {
    double probability =
        static_cast<double>(frequency_distribution[x]) / num_trials;

    EXPECT_NEAR(probability, expected_probability_distribution[x], 0.01);
  }
}

TEST(DiscreteGaussianNoiser, GetNoiseOptionReturnsConstReference) {
  int64_t contributor_count = kContributorCount;
  double sigma_distributed = 1;
  int64_t offset = kOffset;

  DistributedDiscreteGaussianNoiseComponentOptions options{
      contributor_count, sigma_distributed, offset, offset};
  DistributedDiscreteGaussianNoiser distributed_discrete_gaussian_noiser(
      options);
  auto const_noise_options = distributed_discrete_gaussian_noiser.options();
  EXPECT_EQ(const_noise_options.contributor_count, options.contributor_count);
  EXPECT_EQ(const_noise_options.shift_offset, options.shift_offset);
  EXPECT_EQ(const_noise_options.truncate_threshold, options.truncate_threshold);
}

}  // namespace
}  // namespace wfa::math
