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
#include "math/uniform_pseudorandom_generator.h"

namespace wfa::math {

// Key length for EVP_aes_256_ctr.
// See https://www.openssl.org/docs/man1.1.1/man3/EVP_aes_256_ctr.html
inline constexpr int kBytesPerAes256Key = 32;

// IV length is set to the max IV length.
// See https://github.com/openssl/openssl/blob/master/include/openssl/evp.h
inline constexpr int kBytesPerAes256Iv = 16;

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

// A uniform pseudorandom generator based on AES-256 counter mode. This is one
// of the approved Deterministic Random Bit Generators specified in the NIST
// SP 800-90A Rev.1 documentation.
//
// The AES is initialized with a key and an IV. The IV is used as the initial
// counter and has the same length as the block length.
//
// Any implementation of the AES-256 counter mode must be verified against the
// test vectors specified in the NIST SP 800-38A documentation.
class OpenSslUniformPseudorandomGenerator
    : public UniformPseudorandomGenerator {
 public:
  // Create a uniform pseudorandom generator from a key and an IV.
  // The key and IV needs to have the length of kBytesPerAes256Key and
  // kBytesPerAes256Iv respectively.
  static absl::StatusOr<std::unique_ptr<UniformPseudorandomGenerator>> Create(
      const std::vector<unsigned char>& key,
      const std::vector<unsigned char>& iv);

  // Destructor.
  ~OpenSslUniformPseudorandomGenerator() override { EVP_CIPHER_CTX_free(ctx_); }

  // Generate a vector of pseudorandom bytes with the given size.
  absl::StatusOr<std::vector<unsigned char>> GetPseudorandomBytes(
      uint64_t size) override;

  // Generate a vector of pseudorandom values in the range [0, modulus).
  // First, we sample n random values in [0, 2^k) where k is the smallest value
  // such that modulus <= 2^k. Only values in [0, modulus) are accepted. n is
  // chosen such that the expected number of accepted values meets the given
  // `size`.
  absl::StatusOr<std::vector<uint32_t>> GetUniformRandomRange(
      uint64_t size, uint32_t modulus) override;

 private:
  explicit OpenSslUniformPseudorandomGenerator(EVP_CIPHER_CTX* ctx)
      : ctx_(std::move(ctx)) {}
  EVP_CIPHER_CTX* ctx_;
};

}  // namespace wfa::math

#endif  // SRC_MAIN_CC_MATH_OPEN_SSL_UNIFORM_RANDOM_GENERATOR_H_
