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

#include "math/distributed_discrete_gaussian_random_noise.h"

#include <random>

#include "absl/random/bit_gen_ref.h"
#include "absl/random/random.h"

namespace wfa::math {

wfa::math::DistributedDiscreteGaussianRandomNoise::
    DistributedDiscreteGaussianRandomNoise(double sigma)
    : sigma_(sigma) {}

/**
 * Generate a sample from a discrete Gaussian distribution with parameter sigma.
 *
 * This implementation is adapted from Pasin Manurangsi's implementation linked
 * below:
 * https://github.com/world-federation-of-advertisers/cardinality_estimation_evaluation_framework/blob/master/src/common/noisers.py#L207
 */
absl::StatusOr<int64_t>
wfa::math::DistributedDiscreteGaussianRandomNoise::GenerateNoiseComponent() {
  absl::BitGen rnd;
  double sigmaSq = sigma_ * sigma_;
  double t = std::floor(sigma_) + 1;
  double pGeometric = 1 - exp(-1 / t);
  if (pGeometric <= 0 || pGeometric >= 1) {
    return absl::InvalidArgumentError(
        "Probability pGeometric should "
        "be in (0,1).");
  }

  std::geometric_distribution<int> geometric_distribution(pGeometric);
  std::uniform_real_distribution<double> uniform_real_distribution;

  int64_t y;
  double pBernoulli;

  do {
    y = geometric_distribution(rnd) - geometric_distribution(rnd);
    pBernoulli = exp(-pow((std::abs(y) - sigmaSq / t), 2.0) * 0.5 / sigmaSq);
  } while (uniform_real_distribution(rnd) > pBernoulli);

  return y;
}
}  // namespace wfa::math
