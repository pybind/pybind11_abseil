workspace(name = "com_google_pybind11_abseil")
load("@bazel_tools//tools/build_defs/repo:http.bzl", "http_archive")

# To update a PINNED dependency to a new revision,
# a) update URL and strip_prefix to the new git commit hash
# b) get the sha256 hash of the commit by running:
#    curl -L https://github.com/<...>.tar.gz | sha256sum
#    On Mac, run curl -L https://github.com/<...>.tar.gz | shasum -a 256
#    and update the sha256 with the result.

## `bazel_skylib` (PINNED)
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

## `pybind11_bazel` (FLOATING)
# https://github.com/pybind/pybind11_bazel
http_archive(
  name = "pybind11_bazel",
  strip_prefix = "pybind11_bazel-master",
  urls = ["https://github.com/pybind/pybind11_bazel/archive/refs/heads/master.tar.gz"],
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

load("@pybind11_bazel//:python_configure.bzl", "python_configure")
python_configure(name = "local_config_python")
