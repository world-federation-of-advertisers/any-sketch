// Copyright 2020 The Cross-Media Measurement Authors
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

#include "math/noise_parameters_computation.h"

namespace wfa::math {

namespace {

int ComputateMuPolya(double epsilon, double delta, int sensitivity, int n) {
  ABSL_ASSERT(epsilon > 0);
  ABSL_ASSERT(delta > 0);
  ABSL_ASSERT(sensitivity > 0);
  ABSL_ASSERT(n > 0);
  return std::ceil(
      std::log(2.0 * n * sensitivity * (1 + std::exp(epsilon)) / delta) /
      (epsilon / sensitivity));
}

}  // namespace

math::DistributedGeometricRandomComponentOptions GetPublisherNoiseOptions(
    const wfa::any_sketch::DifferentialPrivacyParams& params,
    int publisher_count) {
  ABSL_ASSERT(publisher_count > 0);
  double success_ratio = std::exp(-params.epsilon() / publisher_count);
  int offset =
      ComputateMuPolya(params.epsilon(), params.delta(), publisher_count, 1);
  return {
      .num = 1,
      .p = success_ratio,
      .truncate_threshold = offset,
      .shift_offset = offset,
  };
}

}  // namespace wfa::math
