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

#include <stdexcept>

#include "openssl/rand.h"

namespace wfa::math {

uint64_t OpenSslUniformRandomGenerator::operator()() {
  unsigned char bytes[sizeof(uint64_t)];

  RAND_bytes(bytes, sizeof(bytes));

  return *reinterpret_cast<uint64_t *>(bytes);
}

int OpenSslUniformRandomGenerator::status() { return RAND_status(); }

absl::StatusOr<std::unique_ptr<UniformPseudoRandomGenerator>>
UniformPseudoRandomGenerator::Create(const std::vector<unsigned char>& seed,
                                     const std::vector<unsigned char>& iv) {
  // Check that the seed has the required length.
  if (seed.size() != kBytesPerAES256Seed) {
    return absl::InvalidArgumentError(
        absl::Substitute("The uniform pseudorandom generator seed has "
                         "length of $0 bytes but $1 bytes are required.",
                         seed.size(), kBytesPerAES256Seed));
  }
  // Check that the IV has the required length.
  if (iv.size() != kBytesPerAES256IV) {
    return absl::InvalidArgumentError(
        absl::Substitute("The uniform pseudorandom generator iv has "
                         "length of $0 bytes but $1 bytes are required.",
                         iv.size(), kBytesPerAES256IV));
  }

  // Create new context.
  EVP_CIPHER_CTX *ctx = EVP_CIPHER_CTX_new();
  if (ctx == NULL) {
    return absl::InternalError(
        "Error creating context for the uniform pseudorandom generator.");
  }
  // Initialize the uniform pseudorandom generator with seed and iv.
  if (EVP_EncryptInit_ex(ctx, EVP_aes_256_ctr(), NULL,
                         (const unsigned char *)seed.data(),
                         (const unsigned char *)iv.data()) != 1) {
    return absl::InternalError(
        "Error initializing the uniform pseudorandom generator context.");
  }

  // Using `new` to access a non-public constructor.
  return absl::WrapUnique(new UniformPseudoRandomGenerator(ctx));
}

absl::StatusOr<std::vector<unsigned char>>
UniformPseudoRandomGenerator::GetPseudorandomBytes(int size) {
  if (size <= 0) {
    return absl::InvalidArgumentError(
        "Number of pseudorandom bytes must be a positive value.");
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

}  // namespace wfa::math
