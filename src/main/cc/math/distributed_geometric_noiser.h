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

#ifndef SRC_MAIN_CC_MATH_DISTRIBUTED_GEOMETRIC_NOISER_H_
#define SRC_MAIN_CC_MATH_DISTRIBUTED_GEOMETRIC_NOISER_H_

#include "absl/status/statusor.h"
#include "src/main/cc/math/distributed_noiser.h"
#include "src/main/cc/math/openssl_random_generator.h"

namespace wfa::math {

struct DistributedGeometricNoiseComponentOptions
    : public NoiseComponentOptions {
  explicit DistributedGeometricNoiseComponentOptions(int64_t contributor_count,
                                                     double p,
                                                     int64_t truncate_threshold,
                                                     int64_t shift_offset)
      : NoiseComponentOptions(contributor_count, truncate_threshold,
                              shift_offset),
        p(p) {}
  // The p (success ratio) parameter of the polya distribution. 0<p<1.
  double p;
};

class DistributedGeometricNoiser
    : public DistributedNoiserImpl<DistributedGeometricNoiseComponentOptions> {
 public:
  explicit DistributedGeometricNoiser(
      DistributedGeometricNoiseComponentOptions options);
  [[nodiscard]] absl::StatusOr<int64_t> GenerateNoiseComponent() const override;

 private:
  static constexpr int kMaximumAttempts_ = 20;

  [[nodiscard]] absl::StatusOr<int64_t> GetPolyaRandomVariable(
      double r, double p, OpensslRandomGenerator rnd) const;
  [[nodiscard]] absl::StatusOr<int64_t> GetTruncatedPolyaRandomVariable(
      int64_t truncate_threshold, double r, double p,
      OpensslRandomGenerator rnd) const;
};

}  // namespace wfa::math

#endif  // SRC_MAIN_CC_MATH_DISTRIBUTED_GEOMETRIC_NOISER_H_
