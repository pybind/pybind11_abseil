load("@pybind11_bazel//:build_defs.bzl", "pybind_library")

licenses(["notice"])

package(
    default_visibility = ["//visibility:private"],
)

pybind_library(
    name = "void_ptr_from_capsule",
    srcs = ["void_ptr_from_capsule.cc"],
    hdrs = ["void_ptr_from_capsule.h"],
    visibility = ["//visibility:public"],
    deps = [
        "@com_google_absl//absl/status",
        "@com_google_absl//absl/status:statusor",
        "@com_google_absl//absl/strings",
    ],
)

pybind_library(
    name = "raw_ptr_from_capsule",
    hdrs = ["raw_ptr_from_capsule.h"],
    visibility = ["//visibility:public"],
    deps = [
        ":void_ptr_from_capsule",
        "@com_google_absl//absl/status:statusor",
    ],
)

pybind_library(
    name = "make_shared_ptr_capsule",
    hdrs = ["make_shared_ptr_capsule.h"],
    visibility = ["//visibility:public"],
)

pybind_library(
    name = "shared_ptr_from_capsule",
    hdrs = ["shared_ptr_from_capsule.h"],
    visibility = ["//visibility:public"],
    deps = [
        ":void_ptr_from_capsule",
        "@com_google_absl//absl/status:statusor",
    ],
)
