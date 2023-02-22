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

#include "absl/random/bit_gen_ref.h"
#include "absl/status/statusor.h"
#include "math/distributed_noiser.h"

namespace wfa::math {

struct DistributedGeometricNoiseComponentOptions {
  // For DistributedGeometricNoiser.
  // The number of contributors to the global random variable.
  int64_t contributor_count;
  // The p (success ratio) parameter of the polya distribution. 0<p<1.
  double p;
  // The threshold to truncate the polya random variables. A negative value
  // indicates no truncation.
  int64_t truncate_threshold = -1;
  // The offset added to the PolyaDiff. Usually greater than the
  // truncate_threshold such that the final result is positive.
  int64_t shift_offset = 0;
};

class DistributedGeometricNoiser : public DistributedNoiser {
 public:
  explicit DistributedGeometricNoiser(
      DistributedGeometricNoiseComponentOptions options);
  absl::StatusOr<int64_t> GenerateNoiseComponent() override;

 private:
  DistributedGeometricNoiseComponentOptions options_;
  static constexpr int kMaximumAttempts_ = 20;
  // Truncated Polya random variable
  int64_t polya_{};
  absl::StatusOr<int64_t> GetPolyaRandomVariable(double r, double p,
                                                 absl::BitGenRef rnd);
  absl::StatusOr<int64_t> GetTruncatedPolyaRandomVariable(
      int64_t truncate_threshold, double r, double p, absl::BitGenRef rnd);
};

}  // namespace wfa::math

#endif  // SRC_MAIN_CC_MATH_DISTRIBUTED_GEOMETRIC_NOISER_H_