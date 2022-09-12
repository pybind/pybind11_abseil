
workspace(name = "com_google_pybind11_abseil")
load("@bazel_tools//tools/build_defs/repo:http.bzl", "http_archive")

# To update a dependency to a new revision,
# a) update URL and strip_prefix to the new git commit hash
# b) get the sha256 hash of the commit by running:
#    curl -L https://github.com/<...>.tar.gz | sha256sum
#    and update the sha256 with the result.

http_archive(
    name = "com_google_absl",
    sha256 = "dcf71b9cba8dc0ca9940c4b316a0c796be8fab42b070bb6b7cab62b48f0e66c4",  # SHARED_ABSL_SHA
    strip_prefix = "abseil-cpp-20211102.0",
    urls = [
        "https://github.com/abseil/abseil-cpp/archive/refs/tags/20211102.0.tar.gz"
    ],
)

## `pybind11_bazel`
# See https://github.com/pybind/pybind11_bazel
http_archive(
  name = "pybind11_bazel",
  strip_prefix = "pybind11_bazel-9a24c33cbdc510fa60ab7f5ffb7d80ab89272799",
  sha256 = "32feba648e364da83b0876713cca8a7113f58433cee0680591218dbc202f1392",
  urls = ["https://github.com/pybind/pybind11_bazel/archive/9a24c33cbdc510fa60ab7f5ffb7d80ab89272799.tar.gz"],
)

# We still require the pybind library.
http_archive(
  name = "pybind11",
  build_file = "@pybind11_bazel//:pybind11.BUILD",
  strip_prefix = "pybind11-master",
  urls = ["https://github.com/pybind/pybind11/archive/refs/heads/master.tar.gz"],
)
load("@pybind11_bazel//:python_configure.bzl", "python_configure")
python_configure(name = "local_config_python")
