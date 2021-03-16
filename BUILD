load("@rules_proto//proto:defs.bzl", "proto_library")
load("@rules_cc//cc:defs.bzl", "cc_proto_library")
load("@com_github_grpc_grpc//bazel:cc_grpc_library.bzl", "cc_grpc_library")
load("@rules_cc//cc:defs.bzl", "cc_binary")
load("@rules_cc//cc:defs.bzl", "cc_library")
load("@pybind11_bazel//:build_defs.bzl", "pybind_extension")
COPTS = ["-std=c++17", "-Wno-deprecated", "-O3", "-g"]


cc_binary(
    name = "libpyfeature_extract.so",
    srcs = glob(["pybind.cpp"]),
    copts = COPTS,
    includes = glob(["*.h"]),
    linkopts = ["-lstdc++"],
    linkshared = True,
    linkstatic = True,
    deps = [
        "//src:ads_feature",
        "@pybind11//:pybind11"
    ]
)
