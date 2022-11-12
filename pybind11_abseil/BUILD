# Pybind11 bindings for the Abseil C++ Common Libraries

# LOAD(pytype_pybind_extension)
load("@pybind11_bazel//:build_defs.bzl", "pybind_extension", "pybind_library")

package(default_visibility = ["//visibility:public"])

licenses(["notice"])

exports_files(["LICENSE"])

pybind_library(
    name = "absl_casters",
    hdrs = ["absl_casters.h"],
    deps = [
        "@com_google_absl//absl/cleanup",
        "@com_google_absl//absl/container:btree",
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
        ":raw_ptr_from_capsule",
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

cc_library(
    name = "init_from_tag",
    hdrs = ["init_from_tag.h"],
    visibility = ["//visibility:private"],
)

cc_library(
    name = "raw_ptr_from_capsule",
    srcs = ["raw_ptr_from_capsule.cc"],
    hdrs = ["raw_ptr_from_capsule.h"],
    visibility = ["//visibility:private"],
    deps = [
        "@com_google_absl//absl/status",
        "@com_google_absl//absl/status:statusor",
        "@com_google_absl//absl/strings",
        "@local_config_python//:python_headers",  # buildcleaner: keep
    ],
)

pybind_library(
    name = "utils_pybind11_absl",
    srcs = ["utils_pybind11_absl.cc"],
    hdrs = ["utils_pybind11_absl.h"],
    visibility = ["//visibility:private"],
    deps = [
        "@com_google_absl//absl/strings",
    ],
)

pybind_library(
    name = "register_status_bindings",
    srcs = ["register_status_bindings.cc"],
    hdrs = ["register_status_bindings.h"],
    visibility = ["//visibility:private"],
    deps = [
        ":absl_casters",
        ":init_from_tag",
        ":no_throw_status",
        ":raw_ptr_from_capsule",
        ":status_caster",
        ":status_not_ok_exception",
        ":utils_pybind11_absl",
        "@com_google_absl//absl/status",
        "@com_google_absl//absl/status:statusor",
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
