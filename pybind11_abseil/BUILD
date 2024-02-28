# Pybind11 bindings for the Abseil C++ Common Libraries

# Placeholder: load py_library
load("@pybind11_bazel//:build_defs.bzl", "pybind_library")

package(
    default_visibility = ["//visibility:public"],
)

pybind_library(
    name = "absl_casters",
    hdrs = ["absl_casters.h"],
    deps = [
        "@com_google_absl//absl/cleanup",
        "@com_google_absl//absl/container:btree",
        "@com_google_absl//absl/container:flat_hash_map",
        "@com_google_absl//absl/container:flat_hash_set",
        "@com_google_absl//absl/container:node_hash_map",
        "@com_google_absl//absl/container:node_hash_set",
        "@com_google_absl//absl/strings",
        "@com_google_absl//absl/time",
        "@com_google_absl//absl/types:optional",
        "@com_google_absl//absl/types:span",
    ],
)

cc_library(
    name = "ok_status_singleton_lib",
    srcs = ["ok_status_singleton_lib.cc"],
    hdrs = ["ok_status_singleton_lib.h"],
    visibility = ["//visibility:public"],
    deps = [
        "@com_google_absl//absl/status",
        "@rules_python//python/cc:current_py_cc_headers",  # buildcleaner: keep
    ],
)

cc_library(
    name = "ok_status_singleton_pyinit_google3",
    srcs = ["ok_status_singleton_pyinit_google3.cc"],
    visibility = ["//visibility:private"],
    deps = [
        ":ok_status_singleton_lib",
        "@rules_python//python/cc:current_py_cc_headers",  # buildcleaner: keep
    ],
)

cc_binary(
    name = "ok_status_singleton.so",
    srcs = ["ok_status_singleton_py_extension_stub.cc"],
    linkshared = 1,
    deps = [
        ":ok_status_singleton_pyinit_google3",
        "@rules_python//python/cc:current_py_cc_headers",  # buildcleaner: keep
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
        ":ok_status_singleton_lib",
        ":status_not_ok_exception",
        "//pybind11_abseil/compat:status_from_py_exc",
        "//pybind11_abseil/cpp_capsule_tools:raw_ptr_from_capsule",
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
        "//pybind11_abseil/compat:status_from_py_exc",
        "@com_google_absl//absl/status",
        "@com_google_absl//absl/status:statusor",
    ],
)

cc_library(
    name = "init_from_tag",
    hdrs = ["init_from_tag.h"],
    visibility = ["//visibility:private"],
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
        ":ok_status_singleton_lib",
        ":status_caster",
        ":status_not_ok_exception",
        ":utils_pybind11_absl",
        "//pybind11_abseil/cpp_capsule_tools:raw_ptr_from_capsule",
        "@com_google_absl//absl/status",
        "@com_google_absl//absl/status:statusor",
        "@com_google_absl//absl/strings",
    ],
)

pybind_library(
    name = "status_pyinit_google3",
    srcs = ["status_pyinit_google3.cc"],
    visibility = ["//visibility:private"],
    deps = [
        ":register_status_bindings",
    ],
)

cc_binary(
    name = "status.so",
    srcs = ["status_py_extension_stub.cc"],
    linkopts = select({
        "@platforms//os:osx": [
            "-undefined",
            "dynamic_lookup",
        ],
        "//conditions:default": ["-Wl,-Bsymbolic"],
    }),
    linkshared = 1,
    deps = [
        ":status_pyinit_google3",
        "@rules_python//python/cc:current_py_cc_headers",  # buildcleaner: keep
    ],
)

pybind_library(
    name = "import_status_module",
    srcs = ["import_status_module.cc"],
    hdrs = ["import_status_module.h"],
    data = ["status.so"],
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
