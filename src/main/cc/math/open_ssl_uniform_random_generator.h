// Copyright 2023 The Cross-Media Measurement Authors
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

#ifndef SRC_MAIN_CC_MATH_OPEN_SSL_UNIFORM_RANDOM_GENERATOR_H_
#define SRC_MAIN_CC_MATH_OPEN_SSL_UNIFORM_RANDOM_GENERATOR_H_

#include <openssl/evp.h>
#include <openssl/rand.h>

#include <cstdint>
#include <cstring>
#include <memory>
#include <utility>
#include <vector>

#include "absl/status/status.h"
#include "absl/status/statusor.h"
#include "absl/strings/substitute.h"

namespace wfa::math {

inline constexpr int kBytesPerAES256Seed = 32;
inline constexpr int kBytesPerAES256IV = 16;

// A cryptographically secure uniform random generator.
//
// Satisfies [UniformRandomBitGenerator] that returning unsigned integer values
// such that each value in the range of possible results has (ideally) equal
// probability.
class OpenSslUniformRandomGenerator {
 public:
  OpenSslUniformRandomGenerator() = default;

  using result_type = uint64_t;

  // Check status() before call operator() to ensure the generated value is
  // cryptographically secure.
  result_type operator()();

  static constexpr result_type min() { return 0; }

  static constexpr result_type max() { return UINT64_MAX; }

  // Returns 1 if random generator is seeded successfully with enough entropy.
  int status();
};

class UniformPseudoRandomGenerator {
 public:
  // Create a uniform pseudorandom generator from a seed and an iv.
  // The seed and iv needs to have the length of 32 and 16 bytes respectively.
  static absl::StatusOr<std::unique_ptr<UniformPseudoRandomGenerator>> Create(
      const std::vector<unsigned char>& seed,
      const std::vector<unsigned char>& iv);

  // Destructor.
  ~UniformPseudoRandomGenerator() { EVP_CIPHER_CTX_free(ctx_); }

  // Generate a vector of pseudorandom bytes with the given size.
  absl::StatusOr<std::vector<unsigned char>> GetPseudorandomBytes(int size);

 private:
  explicit UniformPseudoRandomGenerator(EVP_CIPHER_CTX* ctx)
      : ctx_(std::move(ctx)) {}
  EVP_CIPHER_CTX* ctx_;
};

}  // namespace wfa::math

#endif  // SRC_MAIN_CC_MATH_OPEN_SSL_UNIFORM_RANDOM_GENERATOR_H_
