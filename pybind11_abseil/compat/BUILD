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
        "//pybind11_abseil:include_python_h",
        "@abseil-cpp//absl/log:absl_check",
        "@abseil-cpp//absl/log:absl_log",
        "@abseil-cpp//absl/strings",
    ],
)

pybind_library(
    name = "status_from_core_py_exc",
    srcs = ["status_from_core_py_exc.cc"],
    hdrs = ["status_from_core_py_exc.h"],
    visibility = ["//visibility:public"],
    deps = [
        ":py_base_utilities",
        "//pybind11_abseil:include_python_h",
        "@abseil-cpp//absl/container:flat_hash_map",
        "@abseil-cpp//absl/status",
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
        "//pybind11_abseil:include_python_h",
        "//pybind11_abseil/cpp_capsule_tools:raw_ptr_from_capsule",
        "@abseil-cpp//absl/log:absl_check",
        "@abseil-cpp//absl/log:absl_log",
        "@abseil-cpp//absl/status",
    ],
)
