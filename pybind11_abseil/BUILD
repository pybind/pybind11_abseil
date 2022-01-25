# Pybind11 bindings for the Abseil C++ Common Libraries

load("@pybind11_bazel//:build_defs.bzl", "pybind_extension", "pybind_library")

package(default_visibility = ["//visibility:public"])

licenses(["notice"])

exports_files(["LICENSE"])

pybind_library(
    name = "absl_casters",
    hdrs = ["absl_casters.h"],
    deps = [
        "@com_google_absl//absl/cleanup",
        "@com_google_absl//absl/container:flat_hash_map",
        "@com_google_absl//absl/container:flat_hash_set",
        "@com_google_absl//absl/strings",
        "@com_google_absl//absl/time",
        "@com_google_absl//absl/types:optional",
        "@com_google_absl//absl/types:span",
    ],
)

cc_library(
    name = "no_throw_status",
    hdrs = ["no_throw_status.h"],
)

cc_library(
    name = "status_not_ok_exception",
    hdrs = ["status_not_ok_exception.h"],
    deps = ["@com_google_absl//absl/status"],
)

pybind_library(
    name = "status_utils",
    srcs = ["status_utils.cc"],
    hdrs = ["status_utils.h"],
    deps = [
        ":absl_casters",
        ":no_throw_status",
        ":status_not_ok_exception",
        "@com_google_absl//absl/status",
        "@com_google_absl//absl/status:statusor",
    ],
)

pybind_library(
    name = "status_casters",
    hdrs = ["status_casters.h"],
    deps = [
        ":status_utils",
        "@com_google_absl//absl/status",
        "@com_google_absl//absl/status:statusor",
    ],
)

pybind_extension(
    name = "status",
    srcs = ["status.cc"],
    deps = [":status_utils"],
)
