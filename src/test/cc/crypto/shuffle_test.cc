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

#include "crypto/shuffle.h"

#include "absl/status/status.h"
#include "absl/status/statusor.h"
#include "common_cpp/testing/status_macros.h"
#include "common_cpp/testing/status_matchers.h"
#include "gtest/gtest.h"
#include "math/open_ssl_uniform_random_generator.h"

namespace wfa::crypto {
namespace {

using frequency_count::PrngSeed;
using measurement::common::crypto::SecureShuffleWithSeed;
using ::wfa::StatusIs;
using ::wfa::math::kBytesPerAes256Iv;
using ::wfa::math::kBytesPerAes256Key;

TEST(SecureShuffleWithSeed, EmptyInputSucceeds) {
  PrngSeed seed;
  std::vector<uint32_t> data;
  absl::Status ret = SecureShuffleWithSeed(data, seed);
  ASSERT_EQ(ret, absl::OkStatus());
  ASSERT_EQ(data.size(), 0);
}

TEST(SecureShuffleWithSeed, InputHasOneElementSucceeds) {
  PrngSeed seed;
  std::vector<uint32_t> data = {1};
  absl::Status ret = SecureShuffleWithSeed(data, seed);
  ASSERT_EQ(ret, absl::OkStatus());
  ASSERT_EQ(data.size(), 1);
  EXPECT_EQ(data[0], 1);
}

TEST(SecureShuffleWithSeed, InputSizeAtLeastTwoInvalidSeedFails) {
  PrngSeed seed;
  *seed.mutable_key() = std::string(kBytesPerAes256Key - 1, 'a');
  *seed.mutable_iv() = std::string(kBytesPerAes256Iv, 'b');

  std::vector<uint32_t> data = {1, 2};
  absl::Status ret = SecureShuffleWithSeed(data, seed);
  EXPECT_THAT(ret, StatusIs(absl::StatusCode::kInvalidArgument,
                            absl::Substitute(
                                "The uniform pseudorandom generator key has "
                                "length of $0 bytes but $1 bytes are required.",
                                seed.key().size(), kBytesPerAes256Key)));
}

TEST(SecureShuffleWithSeed, ShufflingSucceeds) {
  PrngSeed seed;
  *seed.mutable_key() = std::string(kBytesPerAes256Key, 'a');
  *seed.mutable_iv() = std::string(kBytesPerAes256Iv, 'b');

  int kInputSize = 100;
  std::vector<uint32_t> data(kInputSize);

  for (int i = 0; i < kInputSize; i++) {
    data[i] = i;
  }
  std::vector<uint32_t> input = data;
  absl::Status ret = SecureShuffleWithSeed(data, seed);
  ASSERT_EQ(ret, absl::OkStatus());
  ASSERT_EQ(data.size(), kInputSize);

  // Verifies that the output array is different from the input array.
  // With a random seed, there is a negligible chance of 1/(kInputsize!) that
  // the permutation does not modify the original array and causes this check to
  // fail.
  EXPECT_NE(input, data);

  // Verifies that the input array and the output array have the same elements.
  std::sort(data.begin(), data.end());
  for (int i = 0; i < kInputSize; i++) {
    EXPECT_EQ(data[i], i);
  }
}

TEST(SecureShuffleWithSeed, ShufflingWithSameSeedSucceeds) {
  PrngSeed seed;
  *seed.mutable_key() = std::string(kBytesPerAes256Key, 'a');
  *seed.mutable_iv() = std::string(kBytesPerAes256Iv, 'b');

  int kInputSize = 100;
  std::vector<uint32_t> data_1(kInputSize);

  for (int i = 0; i < kInputSize; i++) {
    data_1[i] = i;
  }

  std::vector<uint32_t> data_2 = data_1;

  absl::Status ret1 = SecureShuffleWithSeed(data_1, seed);
  absl::Status ret2 = SecureShuffleWithSeed(data_2, seed);

  ASSERT_EQ(ret1, absl::OkStatus());
  ASSERT_EQ(ret2, absl::OkStatus());
  ASSERT_EQ(data_1.size(), kInputSize);
  ASSERT_EQ(data_2.size(), kInputSize);

  // Verifies that the two vectors are shuffled using the same permutation.
  EXPECT_EQ(data_1, data_2);
}

}  // namespace
}  // namespace wfa::crypto
