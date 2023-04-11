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

#ifndef SRC_MAIN_CC_MATH_DISTRIBUTED_NOISER_H_
#define SRC_MAIN_CC_MATH_DISTRIBUTED_NOISER_H_

#include "absl/status/statusor.h"

namespace wfa::math {

struct NoiseComponentOptions {
  explicit NoiseComponentOptions(int64_t contributor_count,
                                 int64_t truncate_threshold,
                                 int64_t shift_offset)
      : contributor_count(contributor_count),
        truncate_threshold(truncate_threshold),
        shift_offset(shift_offset) {}
  // The number of contributors to the global random variable.
  int64_t contributor_count;
  // The threshold to truncate the random variables. A negative value
  // indicates no truncation.
  int64_t truncate_threshold;
  // The offset added to the samples. Usually greater than the
  // truncate_threshold such that the final result is positive.
  int64_t shift_offset;
};

class DistributedNoiser {
 public:
  DistributedNoiser(const DistributedNoiser& other) = delete;
  DistributedNoiser& operator=(const DistributedNoiser& other) = delete;
  DistributedNoiser(DistributedNoiser&& other) = delete;
  virtual ~DistributedNoiser() = default;

  [[nodiscard]] virtual absl::StatusOr<int64_t> GenerateNoiseComponent()
      const = 0;
  [[nodiscard]] virtual const NoiseComponentOptions& options() const = 0;

 protected:
  DistributedNoiser() = default;
};

template <typename T>
class DistributedNoiserImpl : public DistributedNoiser {
 public:
  explicit DistributedNoiserImpl(const T& options)
      : DistributedNoiser(), options_(options) {}

  [[nodiscard]] const T& options() const override { return options_; }

 private:
  const T options_;
};

}  // namespace wfa::math
#endif  // SRC_MAIN_CC_MATH_DISTRIBUTED_NOISER_H_
