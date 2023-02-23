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

class DistributedNoiser {
 public:
  DistributedNoiser(const DistributedNoiser& other) = delete;
  DistributedNoiser& operator=(const DistributedNoiser& other) = delete;
  DistributedNoiser(DistributedNoiser&& other) = delete;
  virtual ~DistributedNoiser() = default;

  virtual absl::StatusOr<int64_t> GenerateNoiseComponent() const = 0;

 protected:
  DistributedNoiser() = default;
};

}  // namespace wfa::math
#endif  // SRC_MAIN_CC_MATH_DISTRIBUTED_NOISER_H_
