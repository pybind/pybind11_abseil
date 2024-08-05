workspace(name = "com_google_pybind11_abseil")
load("@bazel_tools//tools/build_defs/repo:http.bzl", "http_archive")

# To update a PINNED dependency to a new revision,
# a) update URL and strip_prefix to the new git commit hash
# b) get the sha256 hash of the commit by running:
#    curl -L https://github.com/<...>.tar.gz | sha256sum
#    On Mac, run curl -L https://github.com/<...>.tar.gz | shasum -a 256
#    and update the sha256 with the result.

################################################################################
#
# WORKSPACE is being deprecated in favor of the new Bzlmod dependency system.
# It will be removed at some point in the future.
#
################################################################################

## `bazel_skylib` (PINNED)
# Needed for Abseil.
http_archive(
    name = "bazel_skylib",  # 2023-05-31T19:24:07Z
    sha256 = "08c0386f45821ce246bbbf77503c973246ed6ee5c3463e41efc197fa9bc3a7f4",
    strip_prefix = "bazel-skylib-288731ef9f7f688932bd50e704a91a45ec185f9b",
    urls = ["https://github.com/bazelbuild/bazel-skylib/archive/288731ef9f7f688932bd50e704a91a45ec185f9b.zip"],
)

## `abseil-cpp` (PINNED)
# https://github.com/abseil/abseil-cpp
http_archive(
    name = "com_google_absl",
    sha256 = "59d2976af9d6ecf001a81a35749a6e551a335b949d34918cfade07737b9d93c5",  # SHARED_ABSL_SHA
    strip_prefix = "abseil-cpp-20230802.0",
    urls = [
        "https://github.com/abseil/abseil-cpp/archive/refs/tags/20230802.0.tar.gz"
    ],
)

http_archive(
    name = "rules_python",
    sha256 = "c68bdc4fbec25de5b5493b8819cfc877c4ea299c0dcb15c244c5a00208cde311",
    strip_prefix = "rules_python-0.31.0",
    url = "https://github.com/bazelbuild/rules_python/releases/download/0.31.0/rules_python-0.31.0.tar.gz",
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


## `pybind11_bazel` (PINNED)
# https://github.com/pybind/pybind11_bazel
http_archive(
  name = "pybind11_bazel",
  strip_prefix = "pybind11_bazel-2.11.1.bzl.2",
  sha256 = "e2ba5f81f3bf6a3fc0417448d49389cc7950bebe48c42c33dfeb4dd59859b9a4",
  urls = ["https://github.com/pybind/pybind11_bazel/releases/download/v2.11.1.bzl.2/pybind11_bazel-2.11.1.bzl.2.tar.gz"],
)

## `pybind11` (FLOATING)
# https://github.com/pybind/pybind11
http_archive(
  name = "pybind11",
  build_file = "@pybind11_bazel//:pybind11.BUILD",
  strip_prefix = "pybind11-master",
  urls = ["https://github.com/pybind/pybind11/archive/refs/heads/master.tar.gz"],
  # For easy local testing with pybind11 releases:
  #   * Comment out the 2 lines above.
  #   * Uncomment and update the 3 lines below.
  #   * To compute the sha256 string:
  #       * Download the .tar.gz file (e.g. curl or wget).
  #       * sha256sum v2.10.4.tar.gz
  # strip_prefix = "pybind11-2.10.4",
  # sha256 = "832e2f309c57da9c1e6d4542dedd34b24e4192ecb4d62f6f4866a737454c9970",
  # urls = ["https://github.com/pybind/pybind11/archive/refs/tags/v2.10.4.tar.gz"],
)
