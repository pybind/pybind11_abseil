# Tests and examples for pybind11_abseil.

load("@pybind11_bazel//:build_defs.bzl", "pybind_extension")

licenses(["notice"])

pybind_extension(
    name = "cpp_capsule_tools_testing",
    srcs = ["cpp_capsule_tools_testing.cc"],
    deps = [
        "//pybind11_abseil/cpp_capsule_tools:make_shared_ptr_capsule",
        "//pybind11_abseil/cpp_capsule_tools:raw_ptr_from_capsule",
        "//pybind11_abseil/cpp_capsule_tools:shared_ptr_from_capsule",
        "@com_google_absl//absl/status:statusor",
    ],
)

py_test(
    name = "cpp_capsule_tools_testing_test",
    srcs = ["cpp_capsule_tools_testing_test.py"],
    data = [":cpp_capsule_tools_testing.so"],
    python_version = "PY3",
    srcs_version = "PY3",
)

cc_library(
    name = "status_testing_no_cpp_eh_lib",
    hdrs = ["status_testing_no_cpp_eh_lib.h"],
    deps = [
        "@com_google_absl//absl/status",
        "@com_google_absl//absl/status:statusor",
        "@local_config_python//:python_headers",  # buildcleaner: keep
    ],
)

pybind_extension(
    name = "status_testing_no_cpp_eh_pybind",
    srcs = ["status_testing_no_cpp_eh_pybind.cc"],
    deps = [
        ":status_testing_no_cpp_eh_lib",
        "//pybind11_abseil:import_status_module",
        "//pybind11_abseil:status_caster",
        "//pybind11_abseil:statusor_caster",
    ],
)

py_library(
    name = "status_testing_no_cpp_eh_test_lib",
    srcs = ["status_testing_no_cpp_eh_test_lib.py"],
)

pybind_extension(
    name = "absl_example",
    srcs = ["absl_example.cc"],
    deps = [
        "//pybind11_abseil:absl_casters",
        "@com_google_absl//absl/container:btree",
        "@com_google_absl//absl/container:flat_hash_set",
        "@com_google_absl//absl/strings",
        "@com_google_absl//absl/time",
        "@com_google_absl//absl/types:optional",
        "@com_google_absl//absl/types:span",
    ],
)

py_test(
    name = "absl_test",
    srcs = ["absl_test.py"],
    data = [":absl_example.so"],
    python_version = "PY3",
    srcs_version = "PY3",
)

py_test(
    name = "ok_status_singleton_test",
    srcs = ["ok_status_singleton_test.py"],
    data = ["//pybind11_abseil:ok_status_singleton.so"],
    python_version = "PY3",
    srcs_version = "PY3",
)

pybind_extension(
    name = "missing_import",
    srcs = ["missing_import.cc"],
    copts = ["-UNDEBUG"],
    deps = [
        "//pybind11_abseil:status_casters",
        "@com_google_absl//absl/status",
        "@com_google_absl//absl/status:statusor",
    ],
)

py_test(
    name = "missing_import_test",
    srcs = ["missing_import_test.py"],
    data = [":missing_import.so"],
    python_version = "PY3",
    srcs_version = "PY3",
)

py_test(
    name = "status_test",
    srcs = ["status_test.py"],
    data = ["//pybind11_abseil:status.so"],
    python_version = "PY3",
    srcs_version = "PY3",
)

pybind_extension(
    name = "status_example",
    srcs = ["status_example.cc"],
    deps = [
        "//pybind11_abseil:status_casters",
        "@com_google_absl//absl/status",
        "@com_google_absl//absl/status:statusor",
    ],
)

py_test(
    name = "status_example_test",
    srcs = ["status_example_test.py"],
    data = [
        ":status_example.so",
        "//pybind11_abseil:status.so",
    ],
    python_version = "PY3",
    srcs_version = "PY3",
)
