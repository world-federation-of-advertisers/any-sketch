// Copyright 2024 The Cross-Media Measurement Authors
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

#include "any_sketch/crypto/shuffle.h"

#include <memory>

#include "absl/status/status.h"
#include "common_cpp/macros/macros.h"
#include "math/open_ssl_uniform_random_generator.h"

namespace wfa::measurement::common::crypto {

absl::Status SecureShuffleWithSeed(std::vector<uint32_t>& data,
                                   const any_sketch::proto::PrngSeed& seed) {
  // Does nothing if the input is empty or has size 1.
  if (data.size() <= 1) {
    return absl::OkStatus();
  }

  // Initializes the pseudorandom generator using the provided seed.
  ASSIGN_OR_RETURN(std::unique_ptr<math::UniformPseudorandomGenerator> prng,
                   math::CreatePrngFromSeed(seed));

  // The custom implementation of Fisher-Yates shuffle is as below. It is not
  // recommended to use std::shuffle because the implementation of std::shuffle
  // is not dictated by the standard, even if an exactly same
  // UniformRandomBitGenertor is used, different results with different standard
  // library implementations could happen.

  // Samples all the random values that will be used to compute all the swapping
  // indices.
  ASSIGN_OR_RETURN(
      std::vector<unsigned char> arr,
      prng->GeneratePseudorandomBytes(data.size() * sizeof(absl::uint128)));

  absl::uint128* rand = (absl::uint128*)arr.data();
  int64_t num_elements = data.size();
  for (int64_t i = 0; i < num_elements - 1; i++) {
    // Ideally, to make sure that the sampled permutation is not biased, rand[i]
    // needs to be re-sampled if rand[i] >= 2^128 - (2^128 % (num_elements -
    // i)). However, the probability that this happens with any i in [1;
    // data.size() - 1] is less than num_elements^2/2^{128}, which is less than
    // 2^{-40} for any input vector of size less than 2^{43}.
    uint64_t index = i + static_cast<uint64_t>(rand[i] % (num_elements - i));
    // Swaps the element at current position with the one at position index.
    std::swap(data[i], data[index]);
  }

  return absl::OkStatus();
}

}  // namespace wfa::measurement::common::crypto
