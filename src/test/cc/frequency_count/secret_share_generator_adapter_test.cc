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

#include "frequency_count/secret_share_generator_adapter.h"

#include "absl/status/status.h"
#include "absl/status/statusor.h"
#include "common_cpp/testing/status_macros.h"
#include "common_cpp/testing/status_matchers.h"
#include "frequency_count/generate_secret_shares.h"
#include "gtest/gtest.h"
#include "math/open_ssl_uniform_random_generator.h"
#include "wfa/frequency_count/secret_share_methods.pb.h"

namespace wfa::frequency_count {
namespace {

using math::OpenSslUniformPseudorandomGenerator;
using math::UniformPseudorandomGenerator;

constexpr int kRingModulus = 128;

TEST(SecretShareAdapterTest, EmptyFrequencyVectorFails) {
  // Build the request
  SecretShareGeneratorRequest request;
  request.set_ring_modulus(kRingModulus);
  std::vector<uint32_t> frequency_vector(0);

  EXPECT_THAT(GenerateSecretShares(request.SerializeAsString()).status(),
              StatusIs(absl::StatusCode::kInvalidArgument, "Input"));
}

TEST(SecretShareAdapterTest, InvalidRingModulusFails) {
  // Build the request
  SecretShareGeneratorRequest request;
  request.set_ring_modulus(1);
  std::vector<uint32_t> frequency_vector = {1, 2, 3, 4, 5};
  request.mutable_data()->Add(frequency_vector.begin(), frequency_vector.end());

  EXPECT_THAT(GenerateSecretShares(request.SerializeAsString()).status(),
              StatusIs(absl::StatusCode::kInvalidArgument, "modulus"));
}

TEST(SecretShareAdapterTest, SecretShareGenerationSucceeds) {
  // Build the request
  SecretShareGeneratorRequest request;
  request.set_ring_modulus(kRingModulus);
  std::vector<uint32_t> frequency_vector = {1, 2, 3, 4, 5};
  request.mutable_data()->Add(frequency_vector.begin(), frequency_vector.end());

  ASSERT_OK_AND_ASSIGN(std::string response,
                       GenerateSecretShares(request.SerializeAsString()));

  SecretShare secret_share;
  secret_share.ParseFromString(response);

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
      prng->GenerateUniformRandomRange(frequency_vector.size(), kRingModulus));

  ASSERT_EQ(secret_share.share_vector().size(), frequency_vector.size());
  ASSERT_EQ(share_vector_from_seed.size(), frequency_vector.size());

  for (int i = 0; i < frequency_vector.size(); i++) {
    ASSERT_EQ(frequency_vector[i],
              (share_vector_from_seed[i] + secret_share.share_vector(i)) %
                  kRingModulus);
  }
}

}  // namespace
}  // namespace wfa::frequency_count
