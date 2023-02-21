// Copyright 2022 The Cross-Media Measurement Authors
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

#include <random>

#include "absl/random/bit_gen_ref.h"
#include "absl/random/random.h"

namespace wfa::math {

DistributedDiscreteGaussianNoiser::DistributedDiscreteGaussianNoiser(
    DistributedDiscreteGaussianNoiseComponentOptions options)
    : options_(options) {}

absl::StatusOr<int64_t>
DistributedDiscreteGaussianNoiser::GenerateNoiseComponent() {
  absl::BitGen rnd;
  double sigma_ = options_.sigma;
  // This simple formula to derive sigma_distributed is valid only for
  // continuous Gaussian and is used as an approximation here.
  double sigma_distributed = sigma_ / sqrt(options_.contributor_count);
  double sigma_sq = sigma_distributed * sigma_distributed;
  double t = std::floor(sigma_) + 1;
  double p_geometric = 1 - exp(-1 / t);
  if (p_geometric <= 0 || p_geometric >= 1) {
    return absl::InvalidArgumentError(
        "Probability p_geometric should "
        "be in (0,1).");
  }

  std::geometric_distribution<int> geometric_distribution(p_geometric);
  std::uniform_real_distribution<double> uniform_real_distribution;

  int64_t y;
  double p_bernoulli;

  do {
    y = geometric_distribution(rnd) - geometric_distribution(rnd);
    p_bernoulli = exp(-pow((std::abs(y) - sigma_sq / t), 2.0) * 0.5 / sigma_sq);
  } while (
      uniform_real_distribution(rnd) > p_bernoulli ||
      (y < -options_.truncate_threshold || y > options_.truncate_threshold));

  return options_.shift_offset + y;
}
}  // namespace wfa::math
