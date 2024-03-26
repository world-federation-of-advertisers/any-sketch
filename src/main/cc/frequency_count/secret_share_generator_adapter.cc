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

#include <string>
#include <vector>

#include "absl/memory/memory.h"
#include "absl/status/status.h"
#include "absl/status/statusor.h"
#include "common_cpp/macros/macros.h"
#include "frequency_count/generate_secret_shares.h"
#include "wfa/frequency_count/secret_share.pb.h"
#include "wfa/frequency_count/secret_share_methods.pb.h"

namespace wfa::frequency_count {

absl::StatusOr<std::string> GenerateSecretShares(
    const std::string& serialized_request) {
  SecretShareGeneratorRequest request_proto;
  if (!request_proto.ParseFromString(serialized_request)) {
    return absl::InvalidArgumentError(
        "failed to parse the SecretShareGeneratorRequest proto.");
  }
  std::vector<uint32_t> frequency_vector = std::vector<uint32_t>(
      request_proto.data().begin(), request_proto.data().end());
  SecretShareParameter secret_share_param;
  secret_share_param.set_modulus(request_proto.ring_modulus());

  ASSIGN_OR_RETURN(auto secret_share,
                   GenerateSecretShares(secret_share_param, frequency_vector));
  return secret_share.SerializeAsString();
}

}  // namespace wfa::frequency_count
