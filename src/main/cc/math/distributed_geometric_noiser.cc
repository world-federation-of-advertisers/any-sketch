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

#include "math/distributed_geometric_noiser.h"

#include "absl/random/poisson_distribution.h"
#include "common_cpp/macros/macros.h"

namespace wfa::math {

DistributedGeometricNoiser::DistributedGeometricNoiser(
    DistributedGeometricNoiseComponentOptions options)
    : DistributedNoiserImpl<DistributedGeometricNoiseComponentOptions>(
          options) {}

absl::StatusOr<int64_t> DistributedGeometricNoiser::GetPolyaRandomVariable(
    double r, double p, OpenSslUniformRandomGenerator rnd) const {
  if (p <= 0 || p >= 1) {
    return absl::InvalidArgumentError("Probability p should be in (0,1).");
  }
  std::gamma_distribution<double> gamma_distribution(r, p / (1 - p));
  absl::poisson_distribution<int> poisson_distribution(gamma_distribution(rnd));
  return poisson_distribution(rnd);
}

absl::StatusOr<int64_t>
DistributedGeometricNoiser::GetTruncatedPolyaRandomVariable(
    int64_t truncate_threshold, double r, double p,
    OpenSslUniformRandomGenerator rnd) const {
  if (truncate_threshold < 0) {
    // Negative truncate_threshold means no truncation.
    return GetPolyaRandomVariable(r, p, rnd);
  }

  for (int i = 0; i < kMaximumAttempts_; ++i) {
    ASSIGN_OR_RETURN(int64_t polya, GetPolyaRandomVariable(r, p, rnd));
    if (polya <= truncate_threshold) {
      return polya;
    }
  }

  return absl::InternalError(
      "Failed to create the polya random variable within the attempt limit.");
}

absl::StatusOr<int64_t> DistributedGeometricNoiser::GenerateNoiseComponent()
    const {
  if (options().contributor_count < 1) {
    return absl::InvalidArgumentError(
        "The contributor_count should be positive.");
  }

  OpenSslUniformRandomGenerator rnd;

  ASSIGN_OR_RETURN(int64_t polya_a,
                   GetTruncatedPolyaRandomVariable(
                       options().truncate_threshold,
                       1.0 / options().contributor_count, options().p, rnd));
  ASSIGN_OR_RETURN(int64_t polya_b,
                   GetTruncatedPolyaRandomVariable(
                       options().truncate_threshold,
                       1.0 / options().contributor_count, options().p, rnd));

  return options().shift_offset + polya_a - polya_b;
}
}  // namespace wfa::math
