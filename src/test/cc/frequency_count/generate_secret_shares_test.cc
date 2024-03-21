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

#include "frequency_count/generate_secret_shares.h"

#include "absl/status/status.h"
#include "absl/status/statusor.h"
#include "common_cpp/testing/status_macros.h"
#include "common_cpp/testing/status_matchers.h"
#include "gtest/gtest.h"
#include "math/open_ssl_uniform_random_generator.h"

namespace wfa::frequency_count {
namespace {

using math::OpenSslUniformPseudorandomGenerator;
using math::UniformPseudorandomGenerator;

TEST(AdditiveSecretSharing, EmptyInputVectorFails) {
  std::vector<uint32_t> input(0);
  SecretShareParameter param;
  param.set_modulus(128);
  auto ret = GenerateSecretShares(param, input);
  EXPECT_THAT(ret.status(), StatusIs(absl::StatusCode::kInvalidArgument,
                                     "Input must be a non-empty vector."));
}

TEST(AdditiveSecretSharing, InvalidModulusFails) {
  std::vector<uint32_t> input = {0, 1, 2};
  SecretShareParameter param;
  param.set_modulus(1);
  auto ret = GenerateSecretShares(param, input);
  EXPECT_THAT(ret.status(), StatusIs(absl::StatusCode::kInvalidArgument,
                                     "The modulus must be greater than 1."));
}

TEST(AdditiveSecretSharing, SecretShareOverRingZ2kElementsSucceeds) {
  std::vector<uint32_t> input = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
  SecretShareParameter param;
  param.set_modulus(128);
  ASSERT_OK_AND_ASSIGN(SecretShare secret_share,
                       GenerateSecretShares(param, input));

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
      prng->GenerateUniformRandomRange(input.size(), param.modulus()));

  ASSERT_EQ(secret_share.share_vector().size(), input.size());
  ASSERT_EQ(share_vector_from_seed.size(), input.size());

  for (int i = 0; i < input.size(); i++) {
    ASSERT_EQ(input[i],
              (share_vector_from_seed[i] + secret_share.share_vector(i)) %
                  param.modulus());
  }
}

TEST(AdditiveSecretSharing, SecretShareOverPrimeFieldElementsSucceeds) {
  std::vector<uint32_t> input = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
  SecretShareParameter param;
  param.set_modulus(127);
  ASSERT_OK_AND_ASSIGN(SecretShare secret_share,
                       GenerateSecretShares(param, input));

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
      prng->GenerateUniformRandomRange(input.size(), param.modulus()));

  ASSERT_EQ(secret_share.share_vector().size(), input.size());
  ASSERT_EQ(share_vector_from_seed.size(), input.size());

  for (int i = 0; i < input.size(); i++) {
    ASSERT_EQ(input[i],
              (share_vector_from_seed[i] + secret_share.share_vector(i)) %
                  param.modulus());
  }
}

TEST(AdditiveSecretSharing, InputOutOfBoundFails) {
  std::vector<uint32_t> input = {0, 1, 2, 3, 4, 5, 6, 7};
  SecretShareParameter param;
  param.set_modulus(7);
  auto secret_share = GenerateSecretShares(param, input);
  EXPECT_THAT(secret_share.status(),
              StatusIs(absl::StatusCode::kInvalidArgument,
                       absl::Substitute(
                           "Inputs must be less than the modulus, which is $0.",
                           param.modulus())));
}

}  // namespace
}  // namespace wfa::frequency_count
