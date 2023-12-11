# Copyright 2023 The Cross-Media Measurement Authors
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#     http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.

"""Module extension for non-module dependencies."""

load("@bazel_tools//tools/build_defs/repo:http.bzl", "http_archive")

def _non_module_deps_impl(
        # buildifier: disable=unused-variable
        mctx):
    http_archive(
        name = "com_google_private_join_and_compute",
        urls = [
            # 2023-07-30
            "https://github.com/google/private-join-and-compute/archive/f77f26fab7f37e5e1e2d43250662c0281bd7fa4a.tar.gz",
        ],
        strip_prefix = "private-join-and-compute-f77f26fab7f37e5e1e2d43250662c0281bd7fa4a",
        sha256 = "18d617a5a66710728f562031c026cf6202d675e222852eaca60d529469464238",
    )

non_module_deps = module_extension(
    implementation = _non_module_deps_impl,
)
