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

#ifndef SRC_MAIN_CC_MATH_OPENSSL_RANDOM_GENERATOR_H_
#define SRC_MAIN_CC_MATH_OPENSSL_RANDOM_GENERATOR_H_

#include <cstring>
#include <random>
#include <stdexcept>

#include "openssl/rand.h"

namespace wfa::math {

class OpensslRandomGenerator : public std::uniform_int_distribution<uint64_t> {
  static const int MAX_ATTEMPTS = 10;

 public:
  OpensslRandomGenerator() = default;

  uint64_t operator()();

  static constexpr uint64_t min() { return 0; }

  static constexpr uint64_t max() { return UINT64_MAX; }
};

}  // namespace wfa::math

#endif  // SRC_MAIN_CC_MATH_OPENSSL_RANDOM_GENERATOR_H_
