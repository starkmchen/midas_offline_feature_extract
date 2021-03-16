load("@rules_proto//proto:defs.bzl", "proto_library")
load("@rules_cc//cc:defs.bzl", "cc_proto_library")
load("@com_github_grpc_grpc//bazel:cc_grpc_library.bzl", "cc_grpc_library")
load("@rules_cc//cc:defs.bzl", "cc_binary")
load("@rules_cc//cc:defs.bzl", "cc_library")
COPTS = ["-std=c++17", "-Wno-deprecated", "-O3"]


proto_library(
    name = "model_feature",
    srcs = ["model_context.proto", "model_count_feature.proto", "model_ad.proto", "model_user_ad.proto", "model_user.proto", "model_feature.proto", "tf_feature.proto"],
    visibility = [
        "//visibility:public",
    ],
)

cc_proto_library(
    name = "model_feature_proto",
    deps = [":model_feature"],
    visibility = ["//visibility:public"],
)

