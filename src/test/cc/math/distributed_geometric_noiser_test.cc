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

#include "math/distributed_geometric_noiser.h"

#include <unordered_map>

#include "absl/status/statusor.h"
#include "common_cpp/macros/macros.h"
#include "common_cpp/testing/status_macros.h"
#include "gtest/gtest.h"

namespace wfa::math {
namespace {

// Create a random number with twoSidedGeometricDistribution using the
// decentralized mechanism, i.e., as the summation of N PolyaDiff.
absl::StatusOr<int64_t> GetTwoSidedGeometricDistributedRandomNumber(
    const DistributedGeometricNoiser& distributed_geometric_noiser,
    int64_t num) {
  int64_t result = 0;
  for (size_t i = 0; i < num; ++i) {
    ASSIGN_OR_RETURN(int64_t temp,
                     distributed_geometric_noiser.GenerateNoiseComponent());
    result += temp;
  }
  return result;
}

TEST(GeometricRandomNoiseIndividualComponent, MeanMaxMinShouldBeCorrect) {
  int64_t shift_offset = 20;
  int64_t truncate_threshold = 10;

  double sum = 0.0;
  int64_t min_value = 1000;
  int64_t max_value = 0;

  size_t num_trials = 100000;

  DistributedGeometricNoiser distributed_geometric_noiser(
      {.contributor_count = 3,
       .p = 0.6,
       .truncate_threshold = truncate_threshold,
       .shift_offset = shift_offset});

  for (size_t i = 0; i < num_trials; ++i) {
    ASSERT_OK_AND_ASSIGN(int64_t temp,
                         distributed_geometric_noiser.GenerateNoiseComponent());
    sum += temp;
    min_value = std::min(min_value, temp);
    max_value = std::max(max_value, temp);
  }
  // Mean should be equal to shift_offset.
  EXPECT_NEAR(sum / num_trials, shift_offset, 0.05);
  // Max should be equal to shift_offset + truncate_threshold
  EXPECT_EQ(max_value, shift_offset + truncate_threshold);
  // Min should be equal to shift_offset - truncate_threshold
  EXPECT_EQ(min_value, shift_offset - truncate_threshold);
}

TEST(GeometricNoiserGlobalSummation, ProbabilityMassFunctionShouldBeCorrect) {
  double p = 0.6;
  int64_t contributor_count = 3;    // 3 contributors
  int64_t shift_offset = 10;        // Individual offset
  int64_t truncate_threshold = 10;  // The value should be reasonably large.

  DistributedGeometricNoiseComponentOptions options = {
      .contributor_count = contributor_count,
      .p = p,
      .truncate_threshold = truncate_threshold,
      .shift_offset = shift_offset};
  auto distributed_geometric_noiser = DistributedGeometricNoiser(options);

  int64_t total_offset = contributor_count * shift_offset;
  int64_t min_output = total_offset - truncate_threshold * contributor_count;
  int64_t max_output = total_offset + truncate_threshold * contributor_count;

  size_t num_trials = 100000;
  std::unordered_map<int64_t, size_t> frequency_distribution;
  for (size_t i = 0; i < num_trials; ++i) {
    ASSERT_OK_AND_ASSIGN(int64_t temp,
                         GetTwoSidedGeometricDistributedRandomNumber(
                             distributed_geometric_noiser, contributor_count));
    ASSERT_GE(temp, min_output);
    ASSERT_LE(temp, max_output);
    ++frequency_distribution[temp];
  }

  for (int64_t x = min_output; x <= max_output; ++x) {
    double probability =
        static_cast<double>(frequency_distribution[x]) / num_trials;
    double expected_probability =
        (1 - p) / (1 + p) * std::pow(p, std::abs(x - total_offset));
    EXPECT_NEAR(probability, expected_probability, 0.01);
  }
}

}  // namespace
}  // namespace wfa::math
