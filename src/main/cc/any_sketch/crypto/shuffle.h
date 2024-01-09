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

#ifndef SRC_MAIN_CC_ANY_SKETCH_CRYPTO_SHUFFLE_H_
#define SRC_MAIN_CC_ANY_SKETCH_CRYPTO_SHUFFLE_H_

#include <string>
#include <utility>

#include "absl/status/status.h"
#include "math/open_ssl_uniform_random_generator.h"
#include "wfa/any_sketch/secret_share.pb.h"

namespace wfa::measurement::common::crypto {

using any_sketch::PrngSeed;
using wfa::math::UniformPseudorandomGenerator;

absl::Status ShuffleWithSeed(std::vector<uint32_t>& data, const PrngSeed& seed);

}  // namespace wfa::measurement::common::crypto

#endif  // SRC_MAIN_CC_ANY_SKETCH_CRYPTO_SHUFFLE_H_
