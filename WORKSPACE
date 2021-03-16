load("@bazel_tools//tools/build_defs/repo:http.bzl", "http_archive")

http_archive(
  name = "pybind11_bazel",
  strip_prefix = "pybind11_bazel-master",
  urls = ["https://github.com/pybind/pybind11_bazel/archive/master.zip"],
)

# We still require the pybind library.
http_archive(
  name = "pybind11",
  build_file = "@pybind11_bazel//:pybind11.BUILD",
  strip_prefix = "pybind11-2.6.1",
  urls = ["https://github.com/pybind/pybind11/archive/v2.6.1.tar.gz"],
)
load("@pybind11_bazel//:python_configure.bzl", "python_configure")
python_configure(name = "local_config_python")


http_archive(
    name = "gtest",
    build_file = "@//:BUILD",
    sha256 = "b58cb7547a28b2c718d1e38aee18a3659c9e3ff52440297e965f5edffe34b6d0",
    strip_prefix = "googletest-release-1.7.0",
    url = "https://github.com/google/googletest/archive/release-1.7.0.zip",
)

http_archive(
    name = "asm",
    build_file = "@//src:BUILD",
    sha256 = "6fc574a759426b5a612f0f1c4f5be1339330641c216ade25344d9daf797f0a2e",
    strip_prefix = "asm-asm-v0.1",
    url = "https://github.com/lkqy/asm/archive/asm-v0.1.zip",
)

http_archive(
    name = "rules_cc",
    urls = ["https://github.com/bazelbuild/rules_cc/archive/bea600c25d246a79398a1c1a73e9261aee35a6b1.tar.gz"],
    sha256 = "b466365e9eb18d509e3f60d970584ebd8c79f3c814a5283ae2cb369f4c678ce8",
    strip_prefix = "rules_cc-bea600c25d246a79398a1c1a73e9261aee35a6b1",
)


http_archive(
    name = "rules_proto",
    urls = ["https://github.com/bazelbuild/rules_proto/archive/40298556293ae502c66579620a7ce867d5f57311.tar.gz"],
    sha256 = "aa1ee19226f707d44bee44c720915199c20c84a23318bb0597ed4e5c873ccbd5",
    strip_prefix = "rules_proto-40298556293ae502c66579620a7ce867d5f57311",
)

load("@rules_cc//cc:repositories.bzl", "rules_cc_dependencies")
rules_cc_dependencies()


http_archive(
    name = "com_github_grpc_grpc",
    sha256 = "d6af0859d3ae4693b1955e972aa2e590d6f4d44baaa82651467c6beea453e30e",
    strip_prefix = "grpc-1.26.0-pre1",
    urls = [
        "https://github.com/grpc/grpc/archive/v1.26.0-pre1.tar.gz",
        ],
)

load("@rules_proto//proto:repositories.bzl", "rules_proto_dependencies", "rules_proto_toolchains")
rules_proto_dependencies()
rules_proto_toolchains()

load("@com_github_grpc_grpc//bazel:grpc_deps.bzl", "grpc_deps")
grpc_deps()

load("@com_github_grpc_grpc//bazel:grpc_extra_deps.bzl", "grpc_extra_deps")
grpc_extra_deps()
