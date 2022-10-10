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

#ifndef SRC_MAIN_CC_MATH_DISTRIBUTED_GEOMETRIC_RANDOM_NOISE_H_
#define SRC_MAIN_CC_MATH_DISTRIBUTED_GEOMETRIC_RANDOM_NOISE_H_

#include "absl/random/bit_gen_ref.h"
#include "src/main/cc/math/distributed_random_noise.h"

namespace wfa::math {

class DistributedGeometricRandomNoise : public DistributedRandomNoise {
 public:
  absl::StatusOr<int64_t> GenerateNoiseComponent(
      DistributedRandomComponentOptions options) override;

 private:
  static constexpr int kMaximumAttempts = 20;
  int64_t polya{};
  absl::StatusOr<int64_t> GetPolyaRandomVariable(double r, double p,
                                                 absl::BitGenRef rnd);
  absl::StatusOr<int64_t> GetTruncatedPolyaRandomVariable(
      int64_t truncate_threshold, double r, double p, absl::BitGenRef rnd);
};

}  // namespace wfa::math

#endif  // SRC_MAIN_CC_MATH_DISTRIBUTED_GEOMETRIC_RANDOM_NOISE_H_
