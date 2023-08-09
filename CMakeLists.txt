cmake_minimum_required(VERSION 3.11)
project(pybind11_abseil LANGUAGES CXX)

include(FetchContent)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CKK_STANDARD_REQUIRED 1)
set(ABSL_PROPAGATE_CXX_STD ON)
set(CMAKE_POSITION_INDEPENDENT_CODE TRUE)
set(CMAKE_INTERPROCEDURAL_OPTIMIZATION FALSE)

find_package(PythonLibs REQUIRED)

FetchContent_declare(
    abseil-cpp
    URL https://github.com/abseil/abseil-cpp/archive/refs/tags/20211102.0.tar.gz
    URL_HASH SHA256=dcf71b9cba8dc0ca9940c4b316a0c796be8fab42b070bb6b7cab62b48f0e66c4
)

FetchContent_declare(
    pybind11
    URL https://github.com/pybind/pybind11/archive/refs/heads/master.tar.gz
)

FetchContent_MakeAvailable(abseil-cpp pybind11)

set(TOP_LEVEL_DIR ${CMAKE_CURRENT_LIST_DIR})
include_directories(${TOP_LEVEL_DIR} ${PYTHON_INCLUDE_DIR} ${pybind11_INCLUDE_DIRS})

add_subdirectory(pybind11_abseil)
