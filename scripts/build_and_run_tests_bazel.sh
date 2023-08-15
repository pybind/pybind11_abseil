#!/bin/bash

# The following scripts:
# - creates a virtualenv
# - installs the pip package dependencies
# - builds and runs tests

set -e  # exit when any command fails
# set -x  # Prints all executed command

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

VIRTUAL_ENV_BINARY=$(which virtualenv || true)
if [[ -z $VIRTUAL_ENV_BINARY || ! -x $VIRTUAL_ENV_BINARY ]]
then
  echo -e -n '\e[1m\e[93m'
  echo -n 'virtualenv command not found '
  echo -n '(try `python3 -m pip install virtualenv`, possibly as root). '
  echo -e 'Exiting...\e[0m'
  exit 1
fi

is_in_virtual_env="false"
# if we are in a virtual_env, we will not create a new one inside.
if [[ "$VIRTUAL_ENV" != "" ]]
then
  echo -e "\e[1m\e[93mVirtualenv already detected. We do not create a new one.\e[0m"
  is_in_virtual_env="true"
fi

echo -e "\e[33mRunning ${0} from $PWD\e[0m"
PYBIN=$(which python3 || true)
if [[ -z $PYBIN || ! -x $PYBIN ]]
then
  echo -e '\e[1m\e[93mpython3 not found! Skip build and test.\e[0m'
  exit 1
fi

PYVERSION=$($PYBIN -c 'import sys; print(".".join(map(str, sys.version_info[:3])))')

VENV_DIR="./venv"

if [[ $is_in_virtual_env == "false" ]]; then
  if ! [ -d "$VENV_DIR" ]; then
    echo "Installing..."
    echo -e "\e[33mInstalling a virtualenv to $VENV_DIR. The setup is long the first time, please wait.\e[0m"
    virtualenv -p $PYBIN $VENV_DIR
  else
    echo -e "\e[33mReusing virtualenv from $VENV_DIR.\e[0m"
  fi
  source $VENV_DIR/bin/activate
fi

# We only exit the virtualenv if we created one.
function cleanup {
  if [[ $is_in_virtual_env == "true" ]]; then
    echo "Exiting virtualenv"
    deactivate
  fi
}
trap cleanup EXIT

echo -e "\e[33mInstalling the requirements (use --noinstall to skip).\e[0m"
pip3 install --upgrade -r ./requirements.txt

echo "Building and testing in $PWD using 'python' (version $PYVERSION)."

export PYTHON_BIN_PATH=`which python3`
export PYTHON_LIB_PATH=`python3 -c "import sysconfig; print(sysconfig.get_path('include'))"`
echo "Using PYTHON_BIN_PATH: $PYTHON_BIN_PATH"
echo "Using PYTHON_LIB_PATH: $PYTHON_LIB_PATH"

BAZEL_CXXOPTS="-std=c++14" bazel test ... --test_output=errors "$@"
BAZEL_CXXOPTS="-std=c++17" bazel test ... --test_output=errors "$@"


