if(USE_SYSTEM_ABSEIL)
  # Version omitted, as absl only allows EXACT version matches
  set(_absl_package_args REQUIRED)
else()
  set(_absl_package_args 20240722)
endif()
if(USE_SYSTEM_PYBIND)
  set(_pybind11_package_args 2.13.6 REQUIRED)
else()
  set(_pybind11_package_args 2.13.6)
endif()

include(FetchContent)
set(BUILD_TESTING OFF)

message(CHECK_START "Checking for external dependencies")
list(APPEND CMAKE_MESSAGE_INDENT "  ")

if(NOT TARGET absl::base)
  message(CHECK_START "Fetching Abseil-cpp")
  list(APPEND CMAKE_MESSAGE_INDENT "  ")
  # ensure that abseil also installs itself, since we are using it in our public API
  set(ABSL_ENABLE_INSTALL ON)
  set(ABSL_PROPAGATE_CXX_STD ON)
  set(ABSL_USE_SYSTEM_INCLUDES ON)
  set(ABSL_BUILD_TESTING OFF)
  FetchContent_Declare(
    absl
    GIT_REPOSITORY "https://github.com/abseil/abseil-cpp.git"
    GIT_TAG "20240722.0"
    GIT_SHALLOW TRUE
    FIND_PACKAGE_ARGS ${_absl_package_args} NAMES absl)
  FetchContent_MakeAvailable(absl)
  list(POP_BACK CMAKE_MESSAGE_INDENT)
  message(CHECK_PASS "fetched")
endif()

if(NOT TARGET pybind11::pybind11_headers)
  message(CHECK_START "Fetching Pybind11")
  list(APPEND CMAKE_MESSAGE_INDENT "  ")
  FetchContent_Declare(
    pybind11
    GIT_REPOSITORY "https://github.com/pybind/pybind11.git"
    GIT_TAG "v2.13.6"
    GIT_SHALLOW TRUE
    FIND_PACKAGE_ARGS ${_pybind11_package_args} NAMES pybind11)
  FetchContent_MakeAvailable(pybind11)
  list(POP_BACK CMAKE_MESSAGE_INDENT)
  message(CHECK_PASS "fetched")
endif()

list(POP_BACK CMAKE_MESSAGE_INDENT)
message(CHECK_PASS "all dependencies found")
