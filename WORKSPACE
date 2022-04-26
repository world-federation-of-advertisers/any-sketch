workspace(name = "any_sketch")

load("@bazel_tools//tools/build_defs/repo:http.bzl", "http_archive")

# Common-cpp
http_archive(
    name = "wfa_common_cpp",
    #    sha256 = "3110be93990a449ac8f60b534319d7d3a08aa118908fecd7b571a5e08260e560",
    strip_prefix = "common-cpp-5f0582b17ffc60d21894aaf2a402518ebe307871",
    url = "https://github.com/world-federation-of-advertisers/common-cpp/archive/5f0582b17ffc60d21894aaf2a402518ebe307871.tar.gz",
)

load("@wfa_common_cpp//build:common_cpp_repositories.bzl", "common_cpp_repositories")

common_cpp_repositories()

load("@wfa_common_cpp//build:common_cpp_deps.bzl", "common_cpp_deps")

common_cpp_deps()

# gRPC
#http_archive(
#    name = "com_github_grpc_grpc",
#    sha256 = "e18b16f7976aab9a36c14c38180f042bb0fd196b75c9fd6a20a2b5f934876ad6",
#    strip_prefix = "grpc-1.45.2",
#    urls = [
#        "https://github.com/grpc/grpc/archive/v1.45.2.tar.gz",
#    ],
#)

load("@com_github_grpc_grpc//bazel:grpc_deps.bzl", "grpc_deps")

# Includes boringssl, com_google_absl, and other dependencies.
grpc_deps()

load("@com_github_grpc_grpc//bazel:grpc_extra_deps.bzl", "grpc_extra_deps")

# Loads transitive dependencies of GRPC.
grpc_extra_deps()
