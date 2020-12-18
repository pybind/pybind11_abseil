
workspace(name = "com_google_pybind11_abseil")
load("@bazel_tools//tools/build_defs/repo:http.bzl", "http_archive")

# To update a dependency to a new revision,
# a) update URL and strip_prefix to the new git commit hash
# b) get the sha256 hash of the commit by running:
#    curl -L https://github.com/<...>.tar.gz | sha256sum
#    and update the sha256 with the result.

http_archive(
    name = "com_google_absl",
    sha256 = "137836d52edb41891cc6d137a228278ae30e76607e3cbd6b8cdb653743c0823e",  # SHARED_ABSL_SHA
    strip_prefix = "abseil-cpp-6df644c56f31b100bf731e27c3825069745651e3",
    urls = [
        "https://github.com/abseil/abseil-cpp/archive/6df644c56f31b100bf731e27c3825069745651e3.tar.gz",
    ],
)

## `pybind11_bazel`
# See https://github.com/pybind/pybind11_bazel
http_archive(
  name = "pybind11_bazel",
  strip_prefix = "pybind11_bazel-26973c0ff320cb4b39e45bc3e4297b82bc3a6c09",
  sha256 = "8f546c03bdd55d0e88cb491ddfbabe5aeb087f87de2fbf441391d70483affe39",
  urls = ["https://github.com/pybind/pybind11_bazel/archive/26973c0ff320cb4b39e45bc3e4297b82bc3a6c09.tar.gz"],
)

# We still require the pybind library.
http_archive(
  name = "pybind11",
  build_file = "@pybind11_bazel//:pybind11.BUILD",
  strip_prefix = "pybind11-2.6",
  sha256 = "22af7c5c65f1ca5d00cbfee5fa9be6aedcaa1ea0c46af22eaa429526e1b88094",
  urls = ["https://github.com/pybind/pybind11/archive/v2.6.tar.gz"],
)
load("@pybind11_bazel//:python_configure.bzl", "python_configure")
python_configure(name = "local_config_python")
