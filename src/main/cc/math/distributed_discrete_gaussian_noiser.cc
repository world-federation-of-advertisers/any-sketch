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
    : DistributedNoiserImpl<DistributedDiscreteGaussianNoiseComponentOptions>(
          options) {}

absl::StatusOr<int64_t>
DistributedDiscreteGaussianNoiser::GenerateNoiseComponent() const {
  absl::BitGen rnd;
  double sigma_distributed = options().sigma_distributed;
  double sigma_sq = sigma_distributed * sigma_distributed;
  double t = std::floor(sigma_distributed) + 1;
  double p_geometric = 1 - exp(-1 / t);
  if (p_geometric <= 0 || p_geometric >= 1) {
    return absl::InvalidArgumentError(
        "Probability p_geometric should "
        "be in (0,1).");
  }
  int64_t truncate_threshold = options().truncate_threshold;

  std::geometric_distribution<int> geometric_distribution(p_geometric);

  int64_t y;
  double p_bernoulli;

  while (true) {
    y = geometric_distribution(rnd) - geometric_distribution(rnd);
    p_bernoulli = exp(-pow((std::abs(y) - sigma_sq / t), 2.0) * 0.5 / sigma_sq);
    std::binomial_distribution<int> binomial_distribution(1, p_bernoulli);
    if (binomial_distribution(rnd) == 1 &&
        (truncate_threshold < 0 ||
         (y >= -truncate_threshold && y <= truncate_threshold)))
      break;
  }

  return options().shift_offset + y;
}
}  // namespace wfa::math
