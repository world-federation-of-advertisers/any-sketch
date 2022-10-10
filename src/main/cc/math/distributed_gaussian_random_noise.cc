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

#include "math/distributed_gaussian_random_noise.h"

#include <random>

#include "absl/random/bit_gen_ref.h"
#include "absl/random/poisson_distribution.h"
#include "absl/random/random.h"
#include "common_cpp/macros/macros.h"

namespace wfa::math {

absl::StatusOr<int64_t>
wfa::math::DistributedGaussianRandomNoise::GenerateNoiseComponent(
    wfa::math::DistributedRandomComponentOptions options) {
  return 100;
}
}  // namespace wfa::math
