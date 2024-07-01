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

using ::testing::IsEmpty;

TEST(OpenSslUniformPseudorandomGenerator,
     CreateTheGeneratorWithValidKeyAndIVSucceeds) {
  std::vector<unsigned char> key(kBytesPerAes256Key);
  std::vector<unsigned char> iv(kBytesPerAes256Iv);
  RAND_bytes(key.data(), key.size());
  RAND_bytes(iv.data(), iv.size());

  ASSERT_OK_AND_ASSIGN(std::unique_ptr<UniformPseudorandomGenerator> prng,
                       OpenSslUniformPseudorandomGenerator::Create(key, iv));
}

TEST(OpenSslUniformPseudorandomGenerator,
     CreateTheGeneratorWithInvalidKeySizeFails) {
  std::vector<unsigned char> key(kBytesPerAes256Key - 1);
  std::vector<unsigned char> iv(kBytesPerAes256Iv);
  RAND_bytes(key.data(), key.size());
  RAND_bytes(iv.data(), iv.size());

  auto prng = OpenSslUniformPseudorandomGenerator::Create(key, iv);
  EXPECT_THAT(
      prng.status(),
      StatusIs(absl::StatusCode::kInvalidArgument,
               absl::Substitute("The uniform pseudorandom generator key has "
                                "length of $0 bytes but $1 bytes are required.",
                                key.size(), kBytesPerAes256Key)));
}

TEST(OpenSslUniformPseudorandomGenerator,
     CreateTheGeneratorWithInvalidIVSizeFails) {
  std::vector<unsigned char> key(kBytesPerAes256Key);
  std::vector<unsigned char> iv(kBytesPerAes256Iv + 1);
  RAND_bytes(key.data(), key.size());
  RAND_bytes(iv.data(), iv.size());

  auto prng = OpenSslUniformPseudorandomGenerator::Create(key, iv);
  EXPECT_THAT(
      prng.status(),
      StatusIs(absl::StatusCode::kInvalidArgument,
               absl::Substitute("The uniform pseudorandom generator IV has "
                                "length of $0 bytes but $1 bytes are required.",
                                iv.size(), kBytesPerAes256Iv)));
}

TEST(OpenSslUniformPseudorandomGenerator,
     CreateTheGeneratorWithValidSeedSucceeds) {
  std::vector<unsigned char> key(kBytesPerAes256Key);
  std::vector<unsigned char> iv(kBytesPerAes256Iv);
  RAND_bytes(key.data(), key.size());
  RAND_bytes(iv.data(), iv.size());

  PrngSeed seed;
  *seed.mutable_key() = std::string(key.begin(), key.end());
  *seed.mutable_iv() = std::string(iv.begin(), iv.end());

  ASSERT_OK_AND_ASSIGN(std::unique_ptr<UniformPseudorandomGenerator> prng,
                       CreatePrngFromSeed(seed));
}

TEST(OpenSslUniformPseudorandomGenerator,
     CreateTheGeneratorFromSeedWithInvalidKeyFails) {
  std::vector<unsigned char> key(kBytesPerAes256Key - 1);
  std::vector<unsigned char> iv(kBytesPerAes256Iv);
  RAND_bytes(key.data(), key.size());
  RAND_bytes(iv.data(), iv.size());

  PrngSeed seed;
  *seed.mutable_key() = std::string(key.begin(), key.end());
  *seed.mutable_iv() = std::string(iv.begin(), iv.end());

  auto prng = CreatePrngFromSeed(seed);
  EXPECT_THAT(
      prng.status(),
      StatusIs(absl::StatusCode::kInvalidArgument,
               absl::Substitute("The uniform pseudorandom generator key has "
                                "length of $0 bytes but $1 bytes are required.",
                                key.size(), kBytesPerAes256Key)));
}

TEST(OpenSslUniformPseudorandomGenerator,
     CreateTheGeneratorFromSeedWithInvalidIVFails) {
  std::vector<unsigned char> key(kBytesPerAes256Key);
  std::vector<unsigned char> iv(kBytesPerAes256Iv - 1);
  RAND_bytes(key.data(), key.size());
  RAND_bytes(iv.data(), iv.size());

  PrngSeed seed;
  *seed.mutable_key() = std::string(key.begin(), key.end());
  *seed.mutable_iv() = std::string(iv.begin(), iv.end());

  auto prng = CreatePrngFromSeed(seed);
  EXPECT_THAT(
      prng.status(),
      StatusIs(absl::StatusCode::kInvalidArgument,
               absl::Substitute("The uniform pseudorandom generator IV has "
                                "length of $0 bytes but $1 bytes are required.",
                                iv.size(), kBytesPerAes256Iv)));
}

TEST(OpenSslUniformPseudorandomGenerator,
     GeneratingNegativeNumberOfRandomBytesFails) {
  std::vector<unsigned char> key(kBytesPerAes256Key);
  std::vector<unsigned char> iv(kBytesPerAes256Iv);
  RAND_bytes(key.data(), key.size());
  RAND_bytes(iv.data(), iv.size());

  ASSERT_OK_AND_ASSIGN(std::unique_ptr<UniformPseudorandomGenerator> prng,
                       OpenSslUniformPseudorandomGenerator::Create(key, iv));
  auto seq = prng->GeneratePseudorandomBytes(-1);
  EXPECT_THAT(seq.status(),
              StatusIs(absl::StatusCode::kInvalidArgument, "negative"));
}

TEST(OpenSslUniformPseudorandomGenerator,
     GeneratingZeroNumberOfRandomBytesSucceeds) {
  std::vector<unsigned char> key(kBytesPerAes256Key);
  std::vector<unsigned char> iv(kBytesPerAes256Iv);
  RAND_bytes(key.data(), key.size());
  RAND_bytes(iv.data(), iv.size());

  ASSERT_OK_AND_ASSIGN(std::unique_ptr<UniformPseudorandomGenerator> prng,
                       OpenSslUniformPseudorandomGenerator::Create(key, iv));
  ASSERT_OK_AND_ASSIGN(auto seq, prng->GeneratePseudorandomBytes(0));
  EXPECT_THAT(seq, IsEmpty());
}

TEST(OpenSslUniformPseudorandomGenerator,
     TwoGeneratorsWithTheSameKeyAndIVProduceTheSameSequence) {
  std::vector<unsigned char> key(kBytesPerAes256Key);
  std::vector<unsigned char> iv(kBytesPerAes256Iv);
  RAND_bytes(key.data(), key.size());
  RAND_bytes(iv.data(), iv.size());

  ASSERT_OK_AND_ASSIGN(std::unique_ptr<UniformPseudorandomGenerator> prng1,
                       OpenSslUniformPseudorandomGenerator::Create(key, iv));

  ASSERT_OK_AND_ASSIGN(std::unique_ptr<UniformPseudorandomGenerator> prng2,
                       OpenSslUniformPseudorandomGenerator::Create(key, iv));

  int kNumRandomBytes = 100;
  ASSERT_OK_AND_ASSIGN(std::vector<unsigned char> seq1,
                       prng1->GeneratePseudorandomBytes(kNumRandomBytes));
  ASSERT_OK_AND_ASSIGN(std::vector<unsigned char> seq2,
                       prng2->GeneratePseudorandomBytes(kNumRandomBytes));

  ASSERT_EQ(seq1.size(), kNumRandomBytes);
  ASSERT_EQ(seq2.size(), kNumRandomBytes);
  ASSERT_EQ(seq1, seq2);
}

TEST(OpenSslUniformPseudorandomGenerator,
     TwoGeneratorsWithTheSameKeyAndIVProduceTheSameSequences) {
  std::vector<unsigned char> key(kBytesPerAes256Key);
  std::vector<unsigned char> iv(kBytesPerAes256Iv);
  RAND_bytes(key.data(), key.size());
  RAND_bytes(iv.data(), iv.size());

  ASSERT_OK_AND_ASSIGN(std::unique_ptr<UniformPseudorandomGenerator> prng1,
                       OpenSslUniformPseudorandomGenerator::Create(key, iv));

  ASSERT_OK_AND_ASSIGN(std::unique_ptr<UniformPseudorandomGenerator> prng2,
                       OpenSslUniformPseudorandomGenerator::Create(key, iv));

  int kNumRandomBytes = 100;
  ASSERT_OK_AND_ASSIGN(std::vector<unsigned char> seq10,
                       prng1->GeneratePseudorandomBytes(kNumRandomBytes));
  ASSERT_OK_AND_ASSIGN(std::vector<unsigned char> seq20,
                       prng2->GeneratePseudorandomBytes(kNumRandomBytes));
  ASSERT_OK_AND_ASSIGN(std::vector<unsigned char> seq11,
                       prng1->GeneratePseudorandomBytes(kNumRandomBytes));
  ASSERT_OK_AND_ASSIGN(std::vector<unsigned char> seq21,
                       prng2->GeneratePseudorandomBytes(kNumRandomBytes));
  ASSERT_EQ(seq10.size(), kNumRandomBytes);
  ASSERT_EQ(seq20.size(), kNumRandomBytes);
  ASSERT_EQ(seq11.size(), kNumRandomBytes);
  ASSERT_EQ(seq21.size(), kNumRandomBytes);
  ASSERT_EQ(seq10, seq20);
  ASSERT_EQ(seq11, seq21);
}

TEST(OpenSslUniformPseudorandomGenerator,
     TwoGeneratorsWithTheSameKeyAndIVProduceTheSameSmallSequences) {
  std::vector<unsigned char> key(kBytesPerAes256Key);
  std::vector<unsigned char> iv(kBytesPerAes256Iv);
  RAND_bytes(key.data(), key.size());
  RAND_bytes(iv.data(), iv.size());

  ASSERT_OK_AND_ASSIGN(std::unique_ptr<UniformPseudorandomGenerator> prng1,
                       OpenSslUniformPseudorandomGenerator::Create(key, iv));

  ASSERT_OK_AND_ASSIGN(std::unique_ptr<UniformPseudorandomGenerator> prng2,
                       OpenSslUniformPseudorandomGenerator::Create(key, iv));

  int kNumRandomBytes = 1;
  ASSERT_OK_AND_ASSIGN(std::vector<unsigned char> seq10,
                       prng1->GeneratePseudorandomBytes(kNumRandomBytes));
  ASSERT_OK_AND_ASSIGN(std::vector<unsigned char> seq20,
                       prng2->GeneratePseudorandomBytes(kNumRandomBytes));
  ASSERT_OK_AND_ASSIGN(std::vector<unsigned char> seq11,
                       prng1->GeneratePseudorandomBytes(kNumRandomBytes));
  ASSERT_OK_AND_ASSIGN(std::vector<unsigned char> seq21,
                       prng2->GeneratePseudorandomBytes(kNumRandomBytes));
  ASSERT_EQ(seq10.size(), kNumRandomBytes);
  ASSERT_EQ(seq20.size(), kNumRandomBytes);
  ASSERT_EQ(seq11.size(), kNumRandomBytes);
  ASSERT_EQ(seq21.size(), kNumRandomBytes);
  ASSERT_EQ(seq10, seq20);
  ASSERT_EQ(seq11, seq21);
}

TEST(OpenSslUniformPseudorandomGenerator,
     TwoGeneratorsWithTheSameKeyAndIVProduceTheSameCombinedSequence) {
  std::vector<unsigned char> key(kBytesPerAes256Key);
  std::vector<unsigned char> iv(kBytesPerAes256Iv);
  RAND_bytes(key.data(), key.size());
  RAND_bytes(iv.data(), iv.size());

  ASSERT_OK_AND_ASSIGN(std::unique_ptr<UniformPseudorandomGenerator> prng1,
                       OpenSslUniformPseudorandomGenerator::Create(key, iv));

  ASSERT_OK_AND_ASSIGN(std::unique_ptr<UniformPseudorandomGenerator> prng2,
                       OpenSslUniformPseudorandomGenerator::Create(key, iv));

  int kNumRandomBytes1 = 45;
  int kNumRandomBytes2 = 55;
  ASSERT_OK_AND_ASSIGN(std::vector<unsigned char> seq10,
                       prng1->GeneratePseudorandomBytes(kNumRandomBytes1));
  ASSERT_OK_AND_ASSIGN(std::vector<unsigned char> seq20,
                       prng2->GeneratePseudorandomBytes(kNumRandomBytes2));
  ASSERT_OK_AND_ASSIGN(std::vector<unsigned char> seq11,
                       prng1->GeneratePseudorandomBytes(kNumRandomBytes2));
  ASSERT_OK_AND_ASSIGN(std::vector<unsigned char> seq21,
                       prng2->GeneratePseudorandomBytes(kNumRandomBytes1));
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

TEST(OpenSslUniformPseudorandomGenerator,
     OpenSslPRNGCompliceWithNISTTestVectorSucceeds) {
  // The NIST's test vectors are defined at
  // https://nvlpubs.nist.gov/nistpubs/legacy/sp/nistspecialpublication800-38a.pdf
  std::vector<unsigned char> kTestKey = {
      0x60, 0x3d, 0xeb, 0x10, 0x15, 0xca, 0x71, 0xbe, 0x2b, 0x73, 0xae,
      0xf0, 0x85, 0x7d, 0x77, 0x81, 0x1f, 0x35, 0x2c, 0x07, 0x3b, 0x61,
      0x08, 0xd7, 0x2d, 0x98, 0x10, 0xa3, 0x09, 0x14, 0xdf, 0xf4};
  std::vector<unsigned char> kTestIv = {0xf0, 0xf1, 0xf2, 0xf3, 0xf4, 0xf5,
                                        0xf6, 0xf7, 0xf8, 0xf9, 0xfa, 0xfb,
                                        0xfc, 0xfd, 0xfe, 0xff};
  std::vector<unsigned char> kOutputBlock1 = {
      0x0b, 0xdf, 0x7d, 0xf1, 0x59, 0x17, 0x16, 0x33,
      0x5e, 0x9a, 0x8b, 0x15, 0xc8, 0x60, 0xc5, 0x02};
  std::vector<unsigned char> kOutputBlock2 = {
      0x5a, 0x6e, 0x69, 0x9d, 0x53, 0x61, 0x19, 0x06,
      0x54, 0x33, 0x86, 0x3c, 0x8f, 0x65, 0x7b, 0x94};
  std::vector<unsigned char> kOutputBlock3 = {
      0x1b, 0xc1, 0x2c, 0x9c, 0x01, 0x61, 0x0d, 0x5d,
      0x0d, 0x8b, 0xd6, 0xa3, 0x37, 0x8e, 0xca, 0x62};
  std::vector<unsigned char> kOutputBlock4 = {
      0x29, 0x56, 0xe1, 0xc8, 0x69, 0x35, 0x36, 0xb1,
      0xbe, 0xe9, 0x9c, 0x73, 0xa3, 0x15, 0x76, 0xb6};

  ASSERT_OK_AND_ASSIGN(
      std::unique_ptr<UniformPseudorandomGenerator> prng,
      OpenSslUniformPseudorandomGenerator::Create(kTestKey, kTestIv));

  int kBlockSize = 16;
  ASSERT_OK_AND_ASSIGN(std::vector<unsigned char> seq1,
                       prng->GeneratePseudorandomBytes(kBlockSize));
  ASSERT_OK_AND_ASSIGN(std::vector<unsigned char> seq2,
                       prng->GeneratePseudorandomBytes(kBlockSize));
  ASSERT_OK_AND_ASSIGN(std::vector<unsigned char> seq3,
                       prng->GeneratePseudorandomBytes(kBlockSize));
  ASSERT_OK_AND_ASSIGN(std::vector<unsigned char> seq4,
                       prng->GeneratePseudorandomBytes(kBlockSize));

  ASSERT_EQ(seq1.size(), kBlockSize);
  ASSERT_EQ(seq2.size(), kBlockSize);
  ASSERT_EQ(seq3.size(), kBlockSize);
  ASSERT_EQ(seq4.size(), kBlockSize);

  ASSERT_EQ(seq1, kOutputBlock1);
  ASSERT_EQ(seq2, kOutputBlock2);
  ASSERT_EQ(seq3, kOutputBlock3);
  ASSERT_EQ(seq4, kOutputBlock4);
}

TEST(OpenSslUniformPseudorandomGenerator,
     GeneratingNegativeNumberOfRandomElementsFails) {
  std::vector<unsigned char> key(kBytesPerAes256Key);
  std::vector<unsigned char> iv(kBytesPerAes256Iv);
  RAND_bytes(key.data(), key.size());
  RAND_bytes(iv.data(), iv.size());
  ASSERT_OK_AND_ASSIGN(std::unique_ptr<UniformPseudorandomGenerator> prng,
                       OpenSslUniformPseudorandomGenerator::Create(key, iv));

  uint32_t kModulus = 128;
  uint64_t kNumRandomElements = -1;
  auto seq = prng->GenerateUniformRandomRange(kNumRandomElements, kModulus);
  EXPECT_THAT(seq.status(),
              StatusIs(absl::StatusCode::kInvalidArgument, "negative"));
}

TEST(OpenSslUniformPseudorandomGenerator,
     GeneratingZeroNumberOfRandomElementsSucceeds) {
  std::vector<unsigned char> key(kBytesPerAes256Key);
  std::vector<unsigned char> iv(kBytesPerAes256Iv);
  RAND_bytes(key.data(), key.size());
  RAND_bytes(iv.data(), iv.size());
  ASSERT_OK_AND_ASSIGN(std::unique_ptr<UniformPseudorandomGenerator> prng,
                       OpenSslUniformPseudorandomGenerator::Create(key, iv));

  uint32_t kModulus = 128;
  uint64_t kNumRandomElements = 0;
  ASSERT_OK_AND_ASSIGN(
      auto seq, prng->GenerateUniformRandomRange(kNumRandomElements, kModulus));
  EXPECT_THAT(seq, IsEmpty());
}

TEST(OpenSslUniformPseudorandomGenerator,
     GeneratingUniformlyRandomWithInvalidModulusFails) {
  std::vector<unsigned char> key(kBytesPerAes256Key);
  std::vector<unsigned char> iv(kBytesPerAes256Iv);
  RAND_bytes(key.data(), key.size());
  RAND_bytes(iv.data(), iv.size());
  ASSERT_OK_AND_ASSIGN(std::unique_ptr<UniformPseudorandomGenerator> prng,
                       OpenSslUniformPseudorandomGenerator::Create(key, iv));

  uint32_t kModulus = 1;
  uint64_t kNumRandomElements = 1;
  auto seq = prng->GenerateUniformRandomRange(kNumRandomElements, kModulus);
  EXPECT_THAT(seq.status(),
              StatusIs(absl::StatusCode::kInvalidArgument, "modulus"));
}

TEST(OpenSslUniformPseudorandomGenerator,
     SampleUniformlyRandomOverRingZ2kElementsSucceeds) {
  std::vector<unsigned char> key(kBytesPerAes256Key);
  std::vector<unsigned char> iv(kBytesPerAes256Iv);
  RAND_bytes(key.data(), key.size());
  RAND_bytes(iv.data(), iv.size());

  ASSERT_OK_AND_ASSIGN(std::unique_ptr<UniformPseudorandomGenerator> prng,
                       OpenSslUniformPseudorandomGenerator::Create(key, iv));
  uint32_t kModulus = 128;
  uint64_t kNumRandomElements = 3661;
  ASSERT_OK_AND_ASSIGN(
      std::vector<uint32_t> seq,
      prng->GenerateUniformRandomRange(kNumRandomElements, kModulus));
  ASSERT_EQ(seq.size(), kNumRandomElements);
  for (int i = 0; i < kNumRandomElements; i++) {
    ASSERT_LT(seq[i], kModulus);
  }
}

TEST(OpenSslUniformPseudorandomGenerator,
     SampleUniformlyRandomOverPrimeFieldElementsSucceeds) {
  std::vector<unsigned char> key(kBytesPerAes256Key);
  std::vector<unsigned char> iv(kBytesPerAes256Iv);
  RAND_bytes(key.data(), key.size());
  RAND_bytes(iv.data(), iv.size());

  ASSERT_OK_AND_ASSIGN(std::unique_ptr<UniformPseudorandomGenerator> prng,
                       OpenSslUniformPseudorandomGenerator::Create(key, iv));
  uint32_t kModulus = 127;
  uint64_t kNumRandomElements = 3661;
  ASSERT_OK_AND_ASSIGN(
      std::vector<uint32_t> seq,
      prng->GenerateUniformRandomRange(kNumRandomElements, kModulus));
  ASSERT_EQ(seq.size(), kNumRandomElements);
  for (int i = 0; i < kNumRandomElements; i++) {
    ASSERT_LT(seq[i], kModulus);
  }
}

TEST(OpenSslUniformPseudorandomGenerator,
     GeneratingNegativeNumberOfNonZeroRandomElementsFails) {
  std::vector<unsigned char> key(kBytesPerAes256Key);
  std::vector<unsigned char> iv(kBytesPerAes256Iv);
  RAND_bytes(key.data(), key.size());
  RAND_bytes(iv.data(), iv.size());
  ASSERT_OK_AND_ASSIGN(std::unique_ptr<UniformPseudorandomGenerator> prng,
                       OpenSslUniformPseudorandomGenerator::Create(key, iv));

  uint32_t kModulus = 128;
  uint64_t kNumRandomElements = -1;
  auto seq =
      prng->GenerateNonZeroUniformRandomRange(kNumRandomElements, kModulus);
  EXPECT_THAT(seq.status(),
              StatusIs(absl::StatusCode::kInvalidArgument, "negative"));
}

TEST(OpenSslUniformPseudorandomGenerator,
     GeneratingZeroNumberOfNonZeroRandomElementsSucceeds) {
  std::vector<unsigned char> key(kBytesPerAes256Key);
  std::vector<unsigned char> iv(kBytesPerAes256Iv);
  RAND_bytes(key.data(), key.size());
  RAND_bytes(iv.data(), iv.size());
  ASSERT_OK_AND_ASSIGN(std::unique_ptr<UniformPseudorandomGenerator> prng,
                       OpenSslUniformPseudorandomGenerator::Create(key, iv));

  uint32_t kModulus = 128;
  uint64_t kNumRandomElements = 0;
  ASSERT_OK_AND_ASSIGN(auto seq, prng->GenerateNonZeroUniformRandomRange(
                                     kNumRandomElements, kModulus));
  EXPECT_THAT(seq, IsEmpty());
}

TEST(OpenSslUniformPseudorandomGenerator,
     GeneratingNonZeroUniformlyRandomWithInvalidModulusFails) {
  std::vector<unsigned char> key(kBytesPerAes256Key);
  std::vector<unsigned char> iv(kBytesPerAes256Iv);
  RAND_bytes(key.data(), key.size());
  RAND_bytes(iv.data(), iv.size());
  ASSERT_OK_AND_ASSIGN(std::unique_ptr<UniformPseudorandomGenerator> prng,
                       OpenSslUniformPseudorandomGenerator::Create(key, iv));

  uint32_t kModulus = 1;
  uint64_t kNumRandomElements = 1;
  auto seq =
      prng->GenerateNonZeroUniformRandomRange(kNumRandomElements, kModulus);
  EXPECT_THAT(seq.status(),
              StatusIs(absl::StatusCode::kInvalidArgument, "modulus"));
}

TEST(OpenSslUniformPseudorandomGenerator,
     SampleNonZeroUniformlyRandomOverRingZ2kElementsSucceeds) {
  std::vector<unsigned char> key(kBytesPerAes256Key);
  std::vector<unsigned char> iv(kBytesPerAes256Iv);
  RAND_bytes(key.data(), key.size());
  RAND_bytes(iv.data(), iv.size());

  ASSERT_OK_AND_ASSIGN(std::unique_ptr<UniformPseudorandomGenerator> prng,
                       OpenSslUniformPseudorandomGenerator::Create(key, iv));
  uint32_t kModulus = 128;
  uint64_t kNumRandomElements = 3661;
  ASSERT_OK_AND_ASSIGN(
      std::vector<uint32_t> seq,
      prng->GenerateNonZeroUniformRandomRange(kNumRandomElements, kModulus));
  ASSERT_EQ(seq.size(), kNumRandomElements);
  for (int i = 0; i < kNumRandomElements; i++) {
    ASSERT_GT(seq[i], 0);
    ASSERT_LT(seq[i], kModulus);
  }
}

TEST(OpenSslUniformPseudorandomGenerator,
     SampleNonZeroUniformlyRandomOverPrimeFieldElementsSucceeds) {
  std::vector<unsigned char> key(kBytesPerAes256Key);
  std::vector<unsigned char> iv(kBytesPerAes256Iv);
  RAND_bytes(key.data(), key.size());
  RAND_bytes(iv.data(), iv.size());

  ASSERT_OK_AND_ASSIGN(std::unique_ptr<UniformPseudorandomGenerator> prng,
                       OpenSslUniformPseudorandomGenerator::Create(key, iv));
  uint32_t kModulus = 127;
  uint64_t kNumRandomElements = 3661;
  ASSERT_OK_AND_ASSIGN(
      std::vector<uint32_t> seq,
      prng->GenerateNonZeroUniformRandomRange(kNumRandomElements, kModulus));
  ASSERT_EQ(seq.size(), kNumRandomElements);
  for (int i = 0; i < kNumRandomElements; i++) {
    ASSERT_GT(seq[i], 0);
    ASSERT_LT(seq[i], kModulus);
  }
}

}  // namespace
}  // namespace wfa::math
