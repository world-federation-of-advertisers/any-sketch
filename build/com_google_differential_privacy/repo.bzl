# Copyright 2022 The Cross-Media Measurement Authors
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#      http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.

"""
Repository rules/macros for Google differential-privacy.
"""

load("@bazel_tools//tools/build_defs/repo:http.bzl", "http_archive")
load("@bazel_tools//tools/build_defs/repo:utils.bzl", "maybe")

def com_google_differential_privacy():
    maybe(
        http_archive,
        name = "com_google_differential_privacy",
        sha256 = "db54f30a4176a07543c54932690afa5932097203cb4be357e9310f99f624886c",
        strip_prefix = "differential-privacy-2.0.0",
        urls = ["https://github.com/google/differential-privacy/archive/refs/tags/v2.0.0.tar.gz"],
    )
    maybe(
        http_archive,
        name = "com_google_cc_differential_privacy",
        sha256 = "db54f30a4176a07543c54932690afa5932097203cb4be357e9310f99f624886c",
        strip_prefix = "differential-privacy-2.0.0/cc",
        urls = ["https://github.com/google/differential-privacy/archive/refs/tags/v2.0.0.tar.gz"],
    )
