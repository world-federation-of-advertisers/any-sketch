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

#ifndef SRC_MAIN_CC_CRYPTO_SHUFFLE_H_
#define SRC_MAIN_CC_CRYPTO_SHUFFLE_H_

#include <string>
#include <utility>
#include <vector>

#include "absl/status/status.h"
#include "wfa/frequency_count/secret_share.pb.h"

namespace wfa::crypto {

// Shuffles the vector data using Fisher-Yates approach. Let n be the size of
// data, the Fisher-Yates shuffle is as below.
// For i = 0 to (n-2):
//   Draws a random value j in the range [i; n-1]
//   Swaps data[i] and data[j]
absl::Status SecureShuffleWithSeed(std::vector<uint32_t>& data,
                                   const frequency_count::PrngSeed& seed);

}  // namespace wfa::crypto

#endif  // SRC_MAIN_CC_CRYPTO_SHUFFLE_H_
