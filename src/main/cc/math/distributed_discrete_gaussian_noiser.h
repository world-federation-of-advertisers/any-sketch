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

#ifndef SRC_MAIN_CC_MATH_DISTRIBUTED_DISCRETE_GAUSSIAN_NOISER_H_
#define SRC_MAIN_CC_MATH_DISTRIBUTED_DISCRETE_GAUSSIAN_NOISER_H_

#include "absl/random/bit_gen_ref.h"
#include "math/distributed_noiser.h"

namespace wfa::math {

struct DistributedDiscreteGaussianNoiseComponentOptions {
  // For DistributedDiscreteGaussianNoiser.
  // The number of contributors to the global random variable.
  int64_t contributor_count;
  // Distributed sigma parameter for discrete Gaussian sampler which is
  // sigma/sqrt(contributor_count).
  double sigma_distributed;
  // The threshold to truncate the discrete Gaussian random variables. A
  // negative value indicates no truncation.
  int64_t truncate_threshold = -1;
  // The offset added to the discrete Gaussian samples. Usually greater than the
  // truncate_threshold such that the final result is positive.
  int64_t shift_offset = 0;
};

class DistributedDiscreteGaussianNoiser : public DistributedNoiser {
 public:
  explicit DistributedDiscreteGaussianNoiser(
      DistributedDiscreteGaussianNoiseComponentOptions options);
  /**
   * Generate a sample from a discrete Gaussian distribution with parameter
   * sigma.
   *
   * This implementation is adapted from pasin30055 implementation linked
   * below. The original algorithm (and analysis) is in the Canone et al's
   * paper, i.e. Algorithm 3 in https://arxiv.org/pdf/2004.00010.pdf.
   * https://github.com/world-federation-of-advertisers/cardinality_estimation_evaluation_framework/blob/master/src/common/noisers.py#L207
   */
  absl::StatusOr<int64_t> GenerateNoiseComponent() override;

 private:
  DistributedDiscreteGaussianNoiseComponentOptions options_;
};
}  // namespace wfa::math

#endif  // SRC_MAIN_CC_MATH_DISTRIBUTED_DISCRETE_GAUSSIAN_NOISER_H_
