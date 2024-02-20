#!/bin/bash

# The following scripts:
# - updates the frozen dependencies
# - builds and runs tests

set -e  # exit when any command fails
set -x  # Prints all executed commands

MYDIR="$(dirname "$(realpath "$0")")"

BAZEL=$(which bazel || true)
if [[ -z $BAZEL || ! -x $BAZEL ]]
then
  echo -e -n '\e[1m\e[93m'
  echo -n 'Bazel not found (bazel (https://bazel.build/) is needed to '
  echo -n 'compile & test). '
  echo -e 'Exiting...\e[0m'
  exit 1
fi

echo "Building and testing in $PWD using 'python' (version $PYVERSION)."

export PYTHON_BIN_PATH=`which python3`
export PYTHON_LIB_PATH=`python3 -c "import sysconfig; print(sysconfig.get_path('include'))"`
echo "Using PYTHON_BIN_PATH: $PYTHON_BIN_PATH"
echo "Using PYTHON_LIB_PATH: $PYTHON_LIB_PATH"

bazel clean --expunge

BAZEL_CXXOPTS="-std=c++17" bazel test ... --test_output=errors "$@" --enable_bzlmod
BAZEL_CXXOPTS="-std=c++20" bazel test ... --test_output=errors "$@" --enable_bzlmod

BAZEL_CXXOPTS="-std=c++17" bazel test ... --test_output=errors "$@" --noenable_bzlmod
BAZEL_CXXOPTS="-std=c++20" bazel test ... --test_output=errors "$@" --noenable_bzlmod
