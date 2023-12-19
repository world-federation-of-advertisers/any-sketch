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
#include "common_cpp/testing/status_macros.h"
#include "common_cpp/testing/status_matchers.h"
#include "gtest/gtest.h"
#include "math/open_ssl_uniform_random_generator.h"

namespace wfa::any_sketch::crypto {
namespace {

using math::OpenSslUniformPseudorandomGenerator;
using math::UniformPseudorandomGenerator;

TEST(AdditiveSecretSharing, EmptyInputVectorFails) {
  std::vector<uint32_t> input(0);
  std::unique_ptr<SecretShareGenerator> secret_share_generator =
      CreateSecretShareGenerator();
  SecretShareParameter param;
  param.set_modulus(128);
  auto ret = secret_share_generator->GenerateSecretShares(param, input);
  EXPECT_THAT(ret.status(), StatusIs(absl::StatusCode::kInvalidArgument,
                                     "Input must be a non-empty vector."));
}

TEST(AdditiveSecretSharing, InvalidModulusFails) {
  std::vector<uint32_t> input = {0, 1, 2};
  std::unique_ptr<SecretShareGenerator> secret_share_generator =
      CreateSecretShareGenerator();
  SecretShareParameter param;
  param.set_modulus(1);
  auto ret = secret_share_generator->GenerateSecretShares(param, input);
  EXPECT_THAT(ret.status(), StatusIs(absl::StatusCode::kInvalidArgument,
                                     "The modulus must be greater than 1."));
}

TEST(AdditiveSecretSharing, SecretShareOverRingZ2kElementsSucceeds) {
  std::vector<uint32_t> input = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
  std::unique_ptr<SecretShareGenerator> secret_share_generator =
      CreateSecretShareGenerator();
  SecretShareParameter param;
  param.set_modulus(128);
  ASSERT_OK_AND_ASSIGN(
      SecretShare secret_share,
      secret_share_generator->GenerateSecretShares(param, input));

  std::string key = secret_share.share_seed().key();
  std::string iv = secret_share.share_seed().iv();

  std::vector<unsigned char> key_vec, iv_vec;
  key_vec.insert(key_vec.end(), key.begin(), key.end());
  iv_vec.insert(iv_vec.end(), iv.begin(), iv.end());

  ASSERT_OK_AND_ASSIGN(
      std::unique_ptr<UniformPseudorandomGenerator> prng,
      OpenSslUniformPseudorandomGenerator::Create(key_vec, iv_vec));

  ASSERT_OK_AND_ASSIGN(
      std::vector<uint32_t> share_vector_from_seed,
      prng->GetUniformRandomRange(input.size(), param.modulus()));

  for (int i = 0; i < input.size(); i++) {
    ASSERT_EQ(input[i],
              (share_vector_from_seed[i] + secret_share.share_vector(i)) %
                  param.modulus());
  }
}

TEST(AdditiveSecretSharing, SecretShareOverPrimeFieldElementsSucceeds) {
  std::vector<uint32_t> input = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
  std::unique_ptr<SecretShareGenerator> secret_share_generator =
      CreateSecretShareGenerator();
  SecretShareParameter param;
  param.set_modulus(127);
  ASSERT_OK_AND_ASSIGN(
      SecretShare secret_share,
      secret_share_generator->GenerateSecretShares(param, input));

  std::string key = secret_share.share_seed().key();
  std::string iv = secret_share.share_seed().iv();

  std::vector<unsigned char> key_vec, iv_vec;
  key_vec.insert(key_vec.end(), key.begin(), key.end());
  iv_vec.insert(iv_vec.end(), iv.begin(), iv.end());

  ASSERT_OK_AND_ASSIGN(
      std::unique_ptr<UniformPseudorandomGenerator> prng,
      OpenSslUniformPseudorandomGenerator::Create(key_vec, iv_vec));

  ASSERT_OK_AND_ASSIGN(
      std::vector<uint32_t> share_vector_from_seed,
      prng->GetUniformRandomRange(input.size(), param.modulus()));

  for (int i = 0; i < input.size(); i++) {
    ASSERT_EQ(input[i],
              (share_vector_from_seed[i] + secret_share.share_vector(i)) %
                  param.modulus());
  }
}

}  // namespace
}  // namespace wfa::any_sketch::crypto