workspace(name = "any_sketch")

load("@bazel_tools//tools/build_defs/repo:http.bzl", "http_archive")

# Common-cpp
http_archive(
    name = "wfa_common_cpp",
    sha256 = "60e9c808d55d14be65347cab008b8bd4f8e2dd8186141609995333bc75fc08ce",
    strip_prefix = "common-cpp-0.8.0",
    url = "https://github.com/world-federation-of-advertisers/common-cpp/archive/v0.8.0.tar.gz",
)

load("@wfa_common_cpp//build:common_cpp_repositories.bzl", "common_cpp_repositories")

common_cpp_repositories()

load("@wfa_common_cpp//build:common_cpp_deps.bzl", "common_cpp_deps")

common_cpp_deps()

load("@wfa_common_cpp//build:common_cpp_extra_deps.bzl", "common_cpp_extra_deps")

common_cpp_extra_deps()

# differential-privacy
http_archive(
    name = "com_google_differential_privacy",
    sha256 = "db54f30a4176a07543c54932690afa5932097203cb4be357e9310f99f624886c",
    strip_prefix = "differential-privacy-2.0.0",
    url = "https://github.com/google/differential-privacy/archive/refs/tags/v2.0.0.tar.gz",
)

http_archive(
    name = "com_google_cc_differential_privacy",
    sha256 = "db54f30a4176a07543c54932690afa5932097203cb4be357e9310f99f624886c",
    strip_prefix = "differential-privacy-2.0.0/cc",
    url = "https://github.com/google/differential-privacy/archive/refs/tags/v2.0.0.tar.gz",
)

load("//build:any_sketch_deps.bzl", "any_sketch_deps")

any_sketch_deps()

load("//build:any_sketch_extra_deps.bzl", "any_sketch_extra_deps")

any_sketch_extra_deps()
