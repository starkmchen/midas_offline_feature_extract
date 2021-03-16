load("@rules_proto//proto:defs.bzl", "proto_library")
load("@rules_cc//cc:defs.bzl", "cc_proto_library")
load("@com_github_grpc_grpc//bazel:cc_grpc_library.bzl", "cc_grpc_library")
load("@rules_cc//cc:defs.bzl", "cc_binary")
load("@rules_cc//cc:defs.bzl", "cc_library")
COPTS = ["-std=c++11", "-Wno-deprecated", "-O3"]

cc_library(
    name = "ads_feature",
    srcs = glob(["ads_feature.cpp"]),
    hdrs = glob(["*.h"]),
    copts = COPTS,
    visibility = ["//visibility:public"],
    deps = [
        "//proto:model_feature_proto",
        "@com_github_grpc_grpc//:grpc++",
    ],
)

# 导出python so
