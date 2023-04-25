// Copyright 2020 The Cross-Media Measurement Authors
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

#include <stdexcept>

#include "openssl/rand.h"

namespace wfa::math {

uint64_t OpenSslUniformRandomGenerator::operator()() {
  unsigned char bytes[sizeof(uint64_t)];

  RAND_bytes(bytes, sizeof(bytes));

  return *reinterpret_cast<uint64_t*>(bytes);
}

int OpenSslUniformRandomGenerator::status() { return RAND_status(); }

}  // namespace wfa::math
