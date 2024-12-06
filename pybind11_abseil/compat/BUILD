load("@pybind11_bazel//:build_defs.bzl", "pybind_library")

licenses(["notice"])

package(
    default_visibility = ["//visibility:private"],
)

pybind_library(
    name = "py_base_utilities",
    srcs = ["py_base_utilities.cc"],
    hdrs = ["py_base_utilities.h"],
    visibility = ["//visibility:public"],
    deps = [
        "@com_google_absl//absl/log:absl_check",
        "@com_google_absl//absl/log:absl_log",
        "@com_google_absl//absl/strings",
    ],
)

pybind_library(
    name = "status_from_core_py_exc",
    srcs = ["status_from_core_py_exc.cc"],
    hdrs = ["status_from_core_py_exc.h"],
    visibility = ["//visibility:public"],
    deps = [
        ":py_base_utilities",
        "@com_google_absl//absl/container:flat_hash_map",
        "@com_google_absl//absl/status",
    ],
)

pybind_library(
    name = "status_from_py_exc",
    srcs = ["status_from_py_exc.cc"],
    hdrs = ["status_from_py_exc.h"],
    visibility = ["//visibility:public"],
    deps = [
        ":py_base_utilities",
        ":status_from_core_py_exc",
        "//pybind11_abseil/cpp_capsule_tools:raw_ptr_from_capsule",
        "@com_google_absl//absl/log:absl_check",
        "@com_google_absl//absl/log:absl_log",
        "@com_google_absl//absl/status",
    ],
)
