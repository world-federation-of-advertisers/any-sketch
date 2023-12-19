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

#include "any_sketch/crypto/secret_share.h"

#include "absl/status/status.h"
#include "absl/status/statusor.h"
#include "common_cpp/macros/macros.h"
#include "math/open_ssl_uniform_random_generator.h"

namespace wfa::any_sketch::crypto {

using wfa::any_sketch::crypto::SecretShareGenerator;
using wfa::math::kBytesPerAes256Iv;
using wfa::math::kBytesPerAes256Key;
using wfa::math::OpenSslUniformPseudorandomGenerator;
using wfa::math::UniformPseudorandomGenerator;

uint32_t SubMod(uint32_t x, uint32_t y, uint32_t modulus) {
  if (x >= y) {
    return x - y;
  } else {
    return x + (modulus - y);
  }
}

class SecretShareGeneratorImpl : public SecretShareGenerator {
 public:
  SecretShareGeneratorImpl() = default;
  ~SecretShareGeneratorImpl() override = default;

  SecretShareGeneratorImpl(SecretShareGeneratorImpl&& other) = delete;
  SecretShareGeneratorImpl& operator=(SecretShareGeneratorImpl&& other) =
      delete;
  SecretShareGeneratorImpl(const SecretShareGeneratorImpl&) = delete;
  SecretShareGeneratorImpl& operator=(const SecretShareGeneratorImpl&) = delete;

  absl::StatusOr<SecretShare> GenerateSecretShares(
      const SecretShareParameter& secret_share_parameter,
      const std::vector<uint32_t>& input) override;
};

absl::StatusOr<SecretShare> SecretShareGeneratorImpl::GenerateSecretShares(
    const SecretShareParameter& secret_share_parameter,
    const std::vector<uint32_t>& input) {
  if (input.size() == 0) {
    return absl::InvalidArgumentError("Input must be a non-empty vector.");
  }

  if (secret_share_parameter.modulus() <= 1) {
    return absl::InvalidArgumentError("The modulus must be greater than 1.");
  }

  // Verify OpenSSL random generator seed has been seeded with enough entropy.
  if (RAND_status() != 1) {
    return absl::InternalError(
        "OpenSSL random generator has been seeded with enough entropy.");
  }

  // Sample random seed as the first share.
  std::vector<unsigned char> key(kBytesPerAes256Key);
  std::vector<unsigned char> iv(kBytesPerAes256Iv);

  if (RAND_bytes(key.data(), kBytesPerAes256Key) != 1) {
    return absl::InternalError("Failed to sample the AES 256 key.");
  }

  if (RAND_bytes(iv.data(), kBytesPerAes256Iv) != 1) {
    return absl::InternalError("Failed to sample the AES 256 IV.");
  }

  // Expanse the seed to get a random string.
  ASSIGN_OR_RETURN(std::unique_ptr<UniformPseudorandomGenerator> prng,
                   OpenSslUniformPseudorandomGenerator::Create(key, iv));

  ASSIGN_OR_RETURN(std::vector<uint32_t> share_vector,
                   prng->GetUniformRandomRange(
                       input.size(), secret_share_parameter.modulus()));
  for (int i = 0; i < input.size(); i++) {
    share_vector[i] =
        SubMod(input[i], share_vector[i], secret_share_parameter.modulus());
  }
  SecretShare secret_share;
  secret_share.mutable_share_vector()->Add(share_vector.begin(),
                                           share_vector.end());
  std::string key_str(key.begin(), key.end());
  std::string iv_str(iv.begin(), iv.end());
  secret_share.mutable_share_seed()->set_key(key_str);
  secret_share.mutable_share_seed()->set_iv(iv_str);

  return secret_share;
}

std::unique_ptr<SecretShareGenerator> CreateSecretShareGenerator() {
  return absl::WrapUnique(new SecretShareGeneratorImpl());
}

}  // namespace wfa::any_sketch::crypto