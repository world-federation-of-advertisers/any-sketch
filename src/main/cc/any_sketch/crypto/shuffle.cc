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

using math::CreatePrngFromSeed;
using math::OpenSslUniformPseudorandomGenerator;
using math::UniformPseudorandomGenerator;

// Shuffles the vector data using Fisher-Yates approach. Let n be the size of
// data, the Fisher-Yates shuffle is as below.
// For i = (n-1) to 1:
//   Draws a random value in the range [0; i]
//   Swaps data[i] and data[j]
absl::Status ShuffleWithSeed(std::vector<uint32_t>& data,
                             const PrngSeed& seed) {
  // Does nothing if the input is empty or has size 1.
  if (data.size() <= 1) {
    return absl::OkStatus();
  }

  // Initializes the pseudorandom generator using the provided seed.
  ASSIGN_OR_RETURN(std::unique_ptr<UniformPseudorandomGenerator> prng,
                   CreatePrngFromSeed(seed));

  // Samples random values.
  ASSIGN_OR_RETURN(
      std::vector<unsigned char> arr,
      prng->GeneratePseudorandomBytes(data.size() * sizeof(unsigned __int128)));

  unsigned __int128* rand = (unsigned __int128*)arr.data();
  for (uint64_t i = data.size() - 1; i >= 1; i--) {
    // Ideally, to make sure that the sampled permutation is not biased, rand[i]
    // needs to be re-sampled if rand[i] >= 2^128 - (2^128 % (i+1)). However,
    // the probability that this happens with any i in [1; data.size() - 1] is
    // less than (data.size())^2/2^{128}, which is less than 2^{-40} for any
    // input vector of size less than 2^{43}.
    int index = rand[i] % (i + 1);
    // Swaps the element at current position with the one at position index.
    uint64_t temp = data[i];
    data[i] = data[index];
    data[index] = temp;
  }

  return absl::OkStatus();
}

}  // namespace wfa::measurement::common::crypto
