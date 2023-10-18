cmake_minimum_required(VERSION 3.11)
project(pybind11_abseil LANGUAGES CXX)

include(FetchContent)
include(CTest)

if(NOT DEFINED CMAKE_CXX_STANDARD)
  set(CMAKE_CXX_STANDARD 17)
endif(NOT DEFINED CMAKE_CXX_STANDARD)
set(ABSL_PROPAGATE_CXX_STD ON)
set(BUILD_TESTING OFF)
set(CMAKE_POSITION_INDEPENDENT_CODE TRUE)
set(CMAKE_INTERPROCEDURAL_OPTIMIZATION FALSE)

find_package(PythonLibs REQUIRED)

FetchContent_Declare(
  abseil-cpp
  URL https://github.com/abseil/abseil-cpp/archive/refs/tags/20230802.0.tar.gz
  URL_HASH
    SHA256=59d2976af9d6ecf001a81a35749a6e551a335b949d34918cfade07737b9d93c5)

FetchContent_Declare(
  pybind11
  URL https://github.com/pybind/pybind11/archive/refs/heads/master.tar.gz)

FetchContent_MakeAvailable(abseil-cpp pybind11)

set(TOP_LEVEL_DIR ${CMAKE_CURRENT_LIST_DIR})
include_directories(${TOP_LEVEL_DIR} ${PYTHON_INCLUDE_DIR}
                    ${pybind11_INCLUDE_DIRS})

add_subdirectory(pybind11_abseil)
