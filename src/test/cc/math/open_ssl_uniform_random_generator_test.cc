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

#include "math/open_ssl_uniform_random_generator.h"

#include "absl/status/status.h"
#include "absl/status/statusor.h"
#include "common_cpp/testing/status_macros.h"
#include "common_cpp/testing/status_matchers.h"
#include "gtest/gtest.h"

namespace wfa::math {
namespace {

TEST(UniformPseudoRandomGenerator,
     CreateTheGeneratorWithValidSeedAndIVSucceeds) {
  std::vector<unsigned char> seed(kBytesPerAES256Seed);
  std::vector<unsigned char> iv(kBytesPerAES256IV);
  RAND_bytes(seed.data(), seed.size());
  RAND_bytes(iv.data(), iv.size());

  ASSERT_OK_AND_ASSIGN(std::unique_ptr<UniformPseudoRandomGenerator> prng,
                       UniformPseudoRandomGenerator::Create(seed, iv));
}

TEST(UniformPseudoRandomGenerator, CreateTheGeneratorWithInvalidSeedSizeFails) {
  std::vector<unsigned char> seed(kBytesPerAES256Seed - 1);
  std::vector<unsigned char> iv(kBytesPerAES256IV);
  RAND_bytes(seed.data(), seed.size());
  RAND_bytes(iv.data(), iv.size());

  auto prng = UniformPseudoRandomGenerator::Create(seed, iv);
  EXPECT_THAT(
      prng.status(),
      StatusIs(absl::StatusCode::kInvalidArgument,
               absl::Substitute("The uniform pseudorandom generator seed has "
                                "length of $0 bytes but $1 bytes are required.",
                                seed.size(), kBytesPerAES256Seed)));
}

TEST(UniformPseudoRandomGenerator, CreateTheGeneratorWithInvalidIVSizeFails) {
  std::vector<unsigned char> seed(kBytesPerAES256Seed);
  std::vector<unsigned char> iv(kBytesPerAES256IV + 1);
  RAND_bytes(seed.data(), seed.size());
  RAND_bytes(iv.data(), iv.size());

  auto prng = UniformPseudoRandomGenerator::Create(seed, iv);
  EXPECT_THAT(
      prng.status(),
      StatusIs(absl::StatusCode::kInvalidArgument,
               absl::Substitute("The uniform pseudorandom generator iv has "
                                "length of $0 bytes but $1 bytes are required.",
                                iv.size(), kBytesPerAES256IV)));
}

TEST(UniformPseudoRandomGenerator,
     GeneratingNonPositiveNumberOfRandomBytesFails) {
  std::vector<unsigned char> seed(kBytesPerAES256Seed);
  std::vector<unsigned char> iv(kBytesPerAES256IV);
  RAND_bytes(seed.data(), seed.size());
  RAND_bytes(iv.data(), iv.size());

  ASSERT_OK_AND_ASSIGN(std::unique_ptr<UniformPseudoRandomGenerator> prng,
                       UniformPseudoRandomGenerator::Create(seed, iv));
  auto seq = prng->GetPseudorandomBytes(0);
  EXPECT_THAT(
      seq.status(),
      StatusIs(absl::StatusCode::kInvalidArgument,
               "Number of pseudorandom bytes must be a positive value."));

  seq = prng->GetPseudorandomBytes(-1);
  EXPECT_THAT(
      seq.status(),
      StatusIs(absl::StatusCode::kInvalidArgument,
               "Number of pseudorandom bytes must be a positive value."));
}

TEST(UniformPseudoRandomGenerator,
     TwoGeneratorsWithTheSameSeedAndIVProduceTheSameSequence) {
  std::vector<unsigned char> seed(kBytesPerAES256Seed);
  std::vector<unsigned char> iv(kBytesPerAES256IV);
  RAND_bytes(seed.data(), seed.size());
  RAND_bytes(iv.data(), iv.size());

  ASSERT_OK_AND_ASSIGN(std::unique_ptr<UniformPseudoRandomGenerator> prng1,
                       UniformPseudoRandomGenerator::Create(seed, iv));

  ASSERT_OK_AND_ASSIGN(std::unique_ptr<UniformPseudoRandomGenerator> prng2,
                       UniformPseudoRandomGenerator::Create(seed, iv));

  int kNumRandomBytes = 100;
  ASSERT_OK_AND_ASSIGN(std::vector<unsigned char> seq1,
                       prng1->GetPseudorandomBytes(kNumRandomBytes));
  ASSERT_OK_AND_ASSIGN(std::vector<unsigned char> seq2,
                       prng2->GetPseudorandomBytes(kNumRandomBytes));

  ASSERT_EQ(seq1.size(), kNumRandomBytes);
  ASSERT_EQ(seq2.size(), kNumRandomBytes);
  ASSERT_EQ(seq1, seq2);
}

TEST(UniformPseudoRandomGenerator,
     TwoGeneratorsWithTheSameSeedAndIVProduceTheSameSequences) {
  std::vector<unsigned char> seed(kBytesPerAES256Seed);
  std::vector<unsigned char> iv(kBytesPerAES256IV);
  RAND_bytes(seed.data(), seed.size());
  RAND_bytes(iv.data(), iv.size());

  ASSERT_OK_AND_ASSIGN(std::unique_ptr<UniformPseudoRandomGenerator> prng1,
                       UniformPseudoRandomGenerator::Create(seed, iv));

  ASSERT_OK_AND_ASSIGN(std::unique_ptr<UniformPseudoRandomGenerator> prng2,
                       UniformPseudoRandomGenerator::Create(seed, iv));

  int kNumRandomBytes = 100;
  ASSERT_OK_AND_ASSIGN(std::vector<unsigned char> seq10,
                       prng1->GetPseudorandomBytes(kNumRandomBytes));
  ASSERT_OK_AND_ASSIGN(std::vector<unsigned char> seq20,
                       prng2->GetPseudorandomBytes(kNumRandomBytes));
  ASSERT_OK_AND_ASSIGN(std::vector<unsigned char> seq11,
                       prng1->GetPseudorandomBytes(kNumRandomBytes));
  ASSERT_OK_AND_ASSIGN(std::vector<unsigned char> seq21,
                       prng2->GetPseudorandomBytes(kNumRandomBytes));
  ASSERT_EQ(seq10.size(), kNumRandomBytes);
  ASSERT_EQ(seq20.size(), kNumRandomBytes);
  ASSERT_EQ(seq11.size(), kNumRandomBytes);
  ASSERT_EQ(seq21.size(), kNumRandomBytes);
  ASSERT_EQ(seq10, seq20);
  ASSERT_EQ(seq11, seq21);
}

TEST(UniformPseudoRandomGenerator,
     TwoGeneratorsWithTheSameSeedAndIVProduceTheSameSmallSequences) {
  std::vector<unsigned char> seed(kBytesPerAES256Seed);
  std::vector<unsigned char> iv(kBytesPerAES256IV);
  RAND_bytes(seed.data(), seed.size());
  RAND_bytes(iv.data(), iv.size());

  ASSERT_OK_AND_ASSIGN(std::unique_ptr<UniformPseudoRandomGenerator> prng1,
                       UniformPseudoRandomGenerator::Create(seed, iv));

  ASSERT_OK_AND_ASSIGN(std::unique_ptr<UniformPseudoRandomGenerator> prng2,
                       UniformPseudoRandomGenerator::Create(seed, iv));

  int kNumRandomBytes = 1;
  ASSERT_OK_AND_ASSIGN(std::vector<unsigned char> seq10,
                       prng1->GetPseudorandomBytes(kNumRandomBytes));
  ASSERT_OK_AND_ASSIGN(std::vector<unsigned char> seq20,
                       prng2->GetPseudorandomBytes(kNumRandomBytes));
  ASSERT_OK_AND_ASSIGN(std::vector<unsigned char> seq11,
                       prng1->GetPseudorandomBytes(kNumRandomBytes));
  ASSERT_OK_AND_ASSIGN(std::vector<unsigned char> seq21,
                       prng2->GetPseudorandomBytes(kNumRandomBytes));
  ASSERT_EQ(seq10.size(), kNumRandomBytes);
  ASSERT_EQ(seq20.size(), kNumRandomBytes);
  ASSERT_EQ(seq11.size(), kNumRandomBytes);
  ASSERT_EQ(seq21.size(), kNumRandomBytes);
  ASSERT_EQ(seq10, seq20);
  ASSERT_EQ(seq11, seq21);
}

TEST(UniformPseudoRandomGenerator,
     TwoGeneratorsWithTheSameSeedAndIVProduceTheSameCombinedSequence) {
  std::vector<unsigned char> seed(kBytesPerAES256Seed);
  std::vector<unsigned char> iv(kBytesPerAES256IV);
  RAND_bytes(seed.data(), seed.size());
  RAND_bytes(iv.data(), iv.size());

  ASSERT_OK_AND_ASSIGN(std::unique_ptr<UniformPseudoRandomGenerator> prng1,
                       UniformPseudoRandomGenerator::Create(seed, iv));

  ASSERT_OK_AND_ASSIGN(std::unique_ptr<UniformPseudoRandomGenerator> prng2,
                       UniformPseudoRandomGenerator::Create(seed, iv));

  int kNumRandomBytes1 = 45;
  int kNumRandomBytes2 = 55;
  ASSERT_OK_AND_ASSIGN(std::vector<unsigned char> seq10,
                       prng1->GetPseudorandomBytes(kNumRandomBytes1));
  ASSERT_OK_AND_ASSIGN(std::vector<unsigned char> seq20,
                       prng2->GetPseudorandomBytes(kNumRandomBytes2));
  ASSERT_OK_AND_ASSIGN(std::vector<unsigned char> seq11,
                       prng1->GetPseudorandomBytes(kNumRandomBytes2));
  ASSERT_OK_AND_ASSIGN(std::vector<unsigned char> seq21,
                       prng2->GetPseudorandomBytes(kNumRandomBytes1));
  ASSERT_EQ(seq10.size(), kNumRandomBytes1);
  ASSERT_EQ(seq20.size(), kNumRandomBytes2);
  ASSERT_EQ(seq11.size(), kNumRandomBytes2);
  ASSERT_EQ(seq21.size(), kNumRandomBytes1);

  std::vector<unsigned char> seq1 = seq10;
  seq1.insert(seq1.end(), seq11.begin(), seq11.end());
  std::vector<unsigned char> seq2 = seq20;
  seq2.insert(seq2.end(), seq21.begin(), seq21.end());
  ASSERT_EQ(seq1, seq2);
}

}  // namespace
}  // namespace wfa::math
