workspace(name = "any_sketch")

load("@bazel_tools//tools/build_defs/repo:http.bzl", "http_archive")

http_archive(
    name = "wfa_common_cpp",
    sha256 = "e0e1f5eed832ef396109354a64c6c1306bf0fb5ea0b449ce6ee1e8edc6fe279d",
    strip_prefix = "common-cpp-43c75acc3394e19bcfd2cfe8e8e2454365d26d60",
    url = "https://github.com/world-federation-of-advertisers/common-cpp/archive/43c75acc3394e19bcfd2cfe8e8e2454365d26d60.tar.gz",
)

load("@wfa_common_cpp//build:deps.bzl", "common_cpp_deps")

common_cpp_deps()

# Protocol buffers
http_archive(
    name = "com_google_protobuf",
    sha256 = "65e020a42bdab44a66664d34421995829e9e79c60e5adaa08282fd14ca552f57",
    strip_prefix = "protobuf-3.15.6",
    urls = [
        "https://github.com/protocolbuffers/protobuf/archive/refs/tags/v3.15.6.tar.gz",
    ],
)

# gRPC
http_archive(
    name = "com_github_grpc_grpc",
    sha256 = "2060769f2d4b0d3535ba594b2ab614d7f68a492f786ab94b4318788d45e3278a",
    strip_prefix = "grpc-1.33.2",
    urls = [
        "https://github.com/grpc/grpc/archive/v1.33.2.tar.gz",
    ],
)

load("@com_github_grpc_grpc//bazel:grpc_deps.bzl", "grpc_deps")

# Includes boringssl, com_google_absl, and other dependencies.
grpc_deps()

load("@com_github_grpc_grpc//bazel:grpc_extra_deps.bzl", "grpc_extra_deps")

# Loads transitive dependencies of GRPC.
grpc_extra_deps()

http_archive(
    name = "com_google_private_join_and_compute",
    sha256 = "13e0414220a2709b0dbeefafe5a4d1b3f3261a541d0405c844857521d5f25f32",
    strip_prefix = "private-join-and-compute-89c8d0aae070b9c282043af419e47d7ef897f460",
    urls = [
        "https://github.com/google/private-join-and-compute/archive/89c8d0aae070b9c282043af419e47d7ef897f460.zip",
    ],
)

new_git_repository(
    name = "farmhash",
    build_file_content = """
package(default_visibility = ["//visibility:public"])
cc_library(
    name = "farmhash",
    hdrs = ["src/farmhash.h"],
    srcs = ["src/farmhash.cc"],
    deps = [],
)""",
    commit = "2f0e005b81e296fa6963e395626137cf729b710c",
    remote = "https://github.com/google/farmhash.git",
    shallow_since = "1509400690 -0700",
)
