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
    name = "check_status_module_imported",
    hdrs = ["check_status_module_imported.h"],
    visibility = ["//visibility:private"],
    deps = [
        "@com_google_absl//absl/status",
    ],
)

pybind_library(
    name = "status_caster",
    hdrs = ["status_caster.h"],
    deps = [
        ":check_status_module_imported",
        ":no_throw_status",
        ":status_not_ok_exception",
        "@com_google_absl//absl/status",
        "@com_google_absl//absl/status:statusor",
    ],
)

pybind_library(
    name = "statusor_caster",
    hdrs = ["statusor_caster.h"],
    deps = [
        ":check_status_module_imported",
        ":no_throw_status",
        ":status_caster",
        "@com_google_absl//absl/status",
        "@com_google_absl//absl/status:statusor",
    ],
)

pybind_library(
    name = "register_status_bindings",
    srcs = ["register_status_bindings.cc"],
    hdrs = ["register_status_bindings.h"],
    visibility = ["//visibility:private"],
    deps = [
        ":absl_casters",
        ":status_not_ok_exception",
        "@com_google_absl//absl/status",
        "@com_google_absl//absl/strings",
    ],
)

pybind_extension(
    name = "status",
    srcs = ["status.cc"],
    deps = [":register_status_bindings"],
)

pybind_library(
    name = "import_status_module",
    srcs = ["import_status_module.cc"],
    hdrs = ["import_status_module.h"],
    deps = [
        ":check_status_module_imported",
        ":register_status_bindings",
        "@com_google_absl//absl/status",
    ],
)

pybind_library(
    name = "status_casters",
    hdrs = ["status_casters.h"],
    deps = [
        ":import_status_module",
        ":status_caster",
        ":statusor_caster",
    ],
)
