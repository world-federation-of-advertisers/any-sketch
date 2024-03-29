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

#ifndef SRC_MAIN_CC_MATH_NOISE_PARAMETERS_COMPUTATION_H_
#define SRC_MAIN_CC_MATH_NOISE_PARAMETERS_COMPUTATION_H_

#include "math/distributed_discrete_gaussian_noiser.h"
#include "math/distributed_geometric_noiser.h"
#include "math/distributed_noiser.h"
#include "wfa/any_sketch/differential_privacy.pb.h"

namespace wfa::math {

DistributedGeometricNoiseComponentOptions GetGeometricPublisherNoiseOptions(
    const wfa::any_sketch::DifferentialPrivacyParams& params,
    int publisher_count);

/**
 * Return a struct of Gaussian publisher noise options.
 *
 * This sigma parameter for discrete Gaussian sampler is computed from
 * DifferentialPrivacyParams epsilon and delta.
 *
 * @param params DifferentialPrivacyParams.
 * @return DistributedDiscreteGaussianNoiseComponentOptions.
 */
DistributedDiscreteGaussianNoiseComponentOptions
GetDiscreteGaussianPublisherNoiseOptions(
    const wfa::any_sketch::DifferentialPrivacyParams& params,
    int64_t contributor_count);

}  // namespace wfa::math

#endif  // SRC_MAIN_CC_MATH_NOISE_PARAMETERS_COMPUTATION_H_
