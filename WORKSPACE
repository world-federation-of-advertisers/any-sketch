workspace(name = "any_sketch")

load("@bazel_tools//tools/build_defs/repo:http.bzl", "http_archive")

# DO_NOT_SUBMIT(world-federation-of-advertisers/common-cpp#34): Replace with version once released from main.
# Common-cpp
http_archive(
    name = "wfa_common_cpp",
    #    sha256 = "60e9c808d55d14be65347cab008b8bd4f8e2dd8186141609995333bc75fc08ce",
    strip_prefix = "common-cpp-8f399b4ee6ecc53c493ea7d2cd3703635c65597f",
    url = "https://github.com/world-federation-of-advertisers/common-cpp/archive/8f399b4ee6ecc53c493ea7d2cd3703635c65597f.tar.gz",
)

load("@wfa_common_cpp//build:common_cpp_repositories.bzl", "common_cpp_repositories")

common_cpp_repositories()

load("@wfa_common_cpp//build:common_cpp_deps.bzl", "common_cpp_deps")

common_cpp_deps()

load("@wfa_common_cpp//build:common_cpp_extra_deps.bzl", "common_cpp_extra_deps")

common_cpp_extra_deps()
