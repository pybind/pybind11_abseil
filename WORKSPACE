workspace(name = "pybind11_abseil")
load("@bazel_tools//tools/build_defs/repo:git.bzl", "git_repository", "new_git_repository")

################################################################################
#
# WORKSPACE is being deprecated in favor of the new Bzlmod dependency system.
# It will be removed at some point in the future.
#
################################################################################

## `bazel_skylib`
# Needed for Abseil.
git_repository(
    name = "bazel_skylib",
    commit = "27d429d8d036af3d010be837cc5924de1ca8d163",
    #tag = "1.7.1",
    remote = "https://github.com/bazelbuild/bazel-skylib.git",
)
load("@bazel_skylib//:workspace.bzl", "bazel_skylib_workspace")
bazel_skylib_workspace()

## Bazel rules...
git_repository(
    name = "platforms",
    commit = "05ec3a3df23fde62471f8288e344cc021dd87bab",
    #tag = "0.0.10",
    remote = "https://github.com/bazelbuild/platforms.git",
)

git_repository(
    name = "rules_java",
    commit = "767e4410850453a10ccf89aa1cededf9de05c72e",
    #tag = "8.6.3",
    remote = "https://github.com/bazelbuild/rules_java.git",
)

load("@rules_java//java:rules_java_deps.bzl", "rules_java_dependencies")
rules_java_dependencies()

# note that the following line is what is minimally required from protobuf for the java rules
# consider using the protobuf_deps() public API from @com_google_protobuf//:protobuf_deps.bzl
load("@com_google_protobuf//bazel/private:proto_bazel_features.bzl", "proto_bazel_features")  # buildifier: disable=bzl-visibility
proto_bazel_features(name = "proto_bazel_features")

# register toolchains
load("@rules_java//java:repositories.bzl", "rules_java_toolchains")
rules_java_toolchains()

## abseil-cpp
# https://github.com/abseil/abseil-cpp
## Abseil-cpp
git_repository(
    name = "com_google_absl",
    commit = "4447c7562e3bc702ade25105912dce503f0c4010",
    #tag = "20240722.0",
    remote = "https://github.com/abseil/abseil-cpp.git",
)

git_repository(
    name = "rules_python",
    commit = "1944874f6ba507f70d8c5e70df84622e0c783254",
    #tag = "0.40.0",
    remote = "https://github.com/bazelbuild/rules_python.git",
)

load("@rules_python//python:repositories.bzl", "py_repositories", "python_register_multi_toolchains")
py_repositories()

load("@rules_python//python/pip_install:repositories.bzl", "pip_install_dependencies")
pip_install_dependencies()

DEFAULT_PYTHON = "3.11"

python_register_multi_toolchains(
    name = "python",
    default_version = DEFAULT_PYTHON,
    python_versions = [
      "3.12",
      "3.11",
      "3.10",
      "3.9",
      "3.8"
    ],
)

load("@python//:pip.bzl", "multi_pip_parse")

multi_pip_parse(
    name = "pypi",
    default_version = DEFAULT_PYTHON,
    python_interpreter_target = {
        "3.12": "@python_3_12_host//:python",
        "3.11": "@python_3_11_host//:python",
        "3.10": "@python_3_10_host//:python",
        "3.9": "@python_3_9_host//:python",
        "3.8": "@python_3_8_host//:python",
    },
    requirements_lock = {
        "3.12": "//pybind11_abseil/requirements:requirements_lock_3_12.txt",
        "3.11": "//pybind11_abseil/requirements:requirements_lock_3_11.txt",
        "3.10": "//pybind11_abseil/requirements:requirements_lock_3_10.txt",
        "3.9": "//pybind11_abseil/requirements:requirements_lock_3_9.txt",
        "3.8": "//pybind11_abseil/requirements:requirements_lock_3_8.txt",
    },
)

load("@pypi//:requirements.bzl", "install_deps")
install_deps()

## `pybind11_bazel`
# https://github.com/pybind/pybind11_bazel
git_repository(
    name = "pybind11_bazel",
    commit = "2b6082a4d9d163a52299718113fa41e4b7978db5",
    #tag = "v2.13.6", # 2024/10/21
    remote = "https://github.com/pybind/pybind11_bazel.git",
)

## `pybind11`
# https://github.com/pybind/pybind11
new_git_repository(
    name = "pybind11",
    build_file = "@pybind11_bazel//:pybind11-BUILD.bazel",
    commit = "a2e59f0e7065404b44dfe92a28aca47ba1378dc4",
    #tag = "v2.13.6",
    remote = "https://github.com/pybind/pybind11.git",
)
