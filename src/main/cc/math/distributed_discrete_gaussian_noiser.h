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
  // sigma parameter for discrete Gaussian sampler.
  double sigma;
};

class DistributedDiscreteGaussianNoiser : public DistributedNoiser {
 public:
  explicit DistributedDiscreteGaussianNoiser(
      DistributedDiscreteGaussianNoiseComponentOptions options);
  absl::StatusOr<int64_t> GenerateNoiseComponent() override;

 private:
  DistributedDiscreteGaussianNoiseComponentOptions options_;
};
}  // namespace wfa::math

#endif  // SRC_MAIN_CC_MATH_DISTRIBUTED_DISCRETE_GAUSSIAN_NOISER_H_
