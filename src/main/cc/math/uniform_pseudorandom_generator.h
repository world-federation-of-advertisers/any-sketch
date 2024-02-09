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

#ifndef SRC_MAIN_CC_MATH_UNIFORM_PSEUDORANDOM_GENERATOR_H_
#define SRC_MAIN_CC_MATH_UNIFORM_PSEUDORANDOM_GENERATOR_H_

#include "openssl/evp.h"
#include "openssl/rand.h"

#include <cstdint>
#include <cstring>
#include <memory>
#include <utility>
#include <vector>

#include "absl/status/status.h"
#include "absl/status/statusor.h"
#include "absl/strings/substitute.h"

namespace wfa::math {

class UniformPseudorandomGenerator {
 public:
  UniformPseudorandomGenerator(const UniformPseudorandomGenerator& other) =
      delete;
  UniformPseudorandomGenerator& operator=(
      const UniformPseudorandomGenerator& other) = delete;
  UniformPseudorandomGenerator(UniformPseudorandomGenerator&& other) = delete;
  virtual ~UniformPseudorandomGenerator() = default;

  // Generates a vector of pseudorandom bytes with the given size.
  virtual absl::StatusOr<std::vector<unsigned char>> GeneratePseudorandomBytes(
      int64_t size) = 0;

  // Generates a vector of pseudorandom values in the range [0, modulus).
  virtual absl::StatusOr<std::vector<uint32_t>> GenerateUniformRandomRange(
      int64_t size, uint32_t modulus) = 0;

 protected:
  UniformPseudorandomGenerator() = default;
};

}  // namespace wfa::math

#endif  // SRC_MAIN_CC_MATH_UNIFORM_PSEUDORANDOM_GENERATOR_H_
