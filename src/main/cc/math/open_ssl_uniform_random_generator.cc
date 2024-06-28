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

#include "math/open_ssl_uniform_random_generator.h"

#include <functional>
#include <stdexcept>

#include "common_cpp/macros/macros.h"
#include "openssl/rand.h"

namespace wfa::math {

uint64_t OpenSslUniformRandomGenerator::operator()() {
  unsigned char bytes[sizeof(uint64_t)];

  RAND_bytes(bytes, sizeof(bytes));

  return *reinterpret_cast<uint64_t *>(bytes);
}

int OpenSslUniformRandomGenerator::status() { return RAND_status(); }

absl::StatusOr<std::unique_ptr<UniformPseudorandomGenerator>>
OpenSslUniformPseudorandomGenerator::Create(
    const std::vector<unsigned char> &key,
    const std::vector<unsigned char> &iv) {
  // Check that the key has the required length.
  if (key.size() != kBytesPerAes256Key) {
    return absl::InvalidArgumentError(
        absl::Substitute("The uniform pseudorandom generator key has "
                         "length of $0 bytes but $1 bytes are required.",
                         key.size(), kBytesPerAes256Key));
  }
  // Check that the IV has the required length.
  if (iv.size() != kBytesPerAes256Iv) {
    return absl::InvalidArgumentError(
        absl::Substitute("The uniform pseudorandom generator IV has "
                         "length of $0 bytes but $1 bytes are required.",
                         iv.size(), kBytesPerAes256Iv));
  }

  // Create new context.
  EVP_CIPHER_CTX *ctx = EVP_CIPHER_CTX_new();
  if (ctx == NULL) {
    return absl::InternalError(
        "Error creating context for the uniform pseudorandom generator.");
  }
  // Initialize the uniform pseudorandom generator with key and IV.
  if (EVP_EncryptInit_ex(ctx, EVP_aes_256_ctr(), NULL,
                         (const unsigned char *)key.data(),
                         (const unsigned char *)iv.data()) != 1) {
    return absl::InternalError(
        "Error initializing the uniform pseudorandom generator context.");
  }

  // Using `new` to access a non-public constructor.
  return absl::WrapUnique(new OpenSslUniformPseudorandomGenerator(ctx));
}

absl::StatusOr<std::vector<unsigned char>>
OpenSslUniformPseudorandomGenerator::GeneratePseudorandomBytes(int64_t size) {
  if (size < 0) {
    return absl::InvalidArgumentError(
        "Number of pseudorandom bytes must be a non-negative value.");
  }

  if (size == 0) {
    return std::vector<unsigned char>();
  }

  std::vector<unsigned char> ret(size, 0);
  int length;
  if (EVP_EncryptUpdate(ctx_, ret.data(), &length, ret.data(), ret.size()) !=
      1) {
    return absl::InternalError(
        "Error updating the uniform pseudorandom generator context.");
  }
  if (EVP_EncryptFinal_ex(ctx_, ret.data() + length, &length) != 1) {
    return absl::InternalError("Error finalizing the generating random bytes.");
  }
  return ret;
}

// Generates uniformly random values in the range [0, modulus) using rejection
// sampling method.
absl::StatusOr<std::vector<uint32_t>>
OpenSslUniformPseudorandomGenerator::GenerateUniformRandomRange(
    int64_t size, uint32_t modulus) {
  if (modulus <= 1) {
    return absl::InvalidArgumentError("The modulus must be greater than 1.");
  }

  if (size < 0) {
    return absl::InvalidArgumentError(
        "Number of pseudorandom elements must be a non-negative value.");
  }

  // Compute the bit length of the modulus.
  int bit_length = std::ceil(std::log2(modulus));
  // The number of bytes needed per element.
  int bytes_per_value = (bit_length + 7) / 8;
  // The mask to extract the last bit_length bits.
  uint32_t mask = (1 << bit_length) - 1;

  // Compute the failure probability, which happens when the sampled value is
  // greater than or equal to modulus. As 2^{bit_length - 1} < modulus <=
  // 2^{bit_length}, the failure probability is guaranteed to be less than 0.5.
  double failure_rate = static_cast<double>((1 << bit_length) - modulus) /
                        static_cast<double>(1 << bit_length);

  std::vector<uint32_t> ret;
  ret.reserve(size);

  while (ret.size() < size) {
    uint64_t current_size = size - ret.size();
    // To get current_size `good` elements, it is expected to sample
    // 1 + current_size*(1 + failure_rate/(1-failure_rate)) elements in
    // [0, 2^{bit_length}).
    uint64_t sample_size = static_cast<uint64_t>(
        current_size + 1.0 + failure_rate * current_size / (1 - failure_rate));

    ASSIGN_OR_RETURN(std::vector<unsigned char> arr,
                     GeneratePseudorandomBytes(sample_size * bytes_per_value));

    // Rejection sampling step.
    for (uint64_t i = 0; i < arr.size(); i++) {
      if (ret.size() >= size) {
        break;
      }
      uint32_t temp = 0;
      for (int j = 0; j < bytes_per_value; j++) {
        temp = (temp << 8) + arr[i * bytes_per_value + j];
      }
      temp &= mask;

      // Accept the value if it is less than modulus.
      if (temp < modulus) {
        ret.push_back(temp);
      }
    }
  }

  return ret;
}

// Generates uniformly random values in the range [1, modulus).
absl::StatusOr<std::vector<uint32_t>>
OpenSslUniformPseudorandomGenerator::GenerateNonZeroUniformRandomRange(
    int64_t size, uint32_t modulus) {
  if (modulus <= 1) {
    return absl::InvalidArgumentError("The modulus must be greater than 1.");
  }

  if (size < 0) {
    return absl::InvalidArgumentError(
        "Number of pseudorandom elements must be a non-negative value.");
  }

  std::vector<uint32_t> ret;
  ret.reserve(size);
  // Compute the failure chance, which happens when the sampled value is 0.
  double failure_rate = 1.0 / static_cast<double>(modulus);
  while (ret.size() < size) {
    int64_t current_size = size - ret.size();
    // To get current_size `good` elements, it is expected to sample
    // 1 + current_size*(1 + failure_rate/(1-failure_rate)) elements.
    int64_t sample_size = static_cast<int64_t>(
        current_size + 1.0 + failure_rate * current_size / (1 - failure_rate));
    ASSIGN_OR_RETURN(std::vector<uint32_t> arr,
                     GenerateUniformRandomRange(sample_size, modulus));
    // Rejection sampling step.
    for (int64_t i = 0; i < arr.size(); i++) {
      if (ret.size() >= size) {
        break;
      }

      // Accept the value if the element is not zero.
      if (arr[i] > 0) {
        ret.push_back(arr[i]);
      }
    }
  }
  return ret;
}

absl::StatusOr<std::unique_ptr<UniformPseudorandomGenerator>>
CreatePrngFromSeed(const PrngSeed &seed) {
  // TODO(@ple13): use absl::Cord instead of making vector copies once we can
  // upgrade to Protobuf v23.0+.
  std::vector<unsigned char> key(seed.key().begin(), seed.key().end());
  std::vector<unsigned char> iv(seed.iv().begin(), seed.iv().end());

  ASSIGN_OR_RETURN(std::unique_ptr<UniformPseudorandomGenerator> prng,
                   OpenSslUniformPseudorandomGenerator::Create(key, iv));
  return prng;
}

}  // namespace wfa::math
