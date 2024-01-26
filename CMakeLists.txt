cmake_minimum_required(VERSION 3.16)
project(pybind11_abseil LANGUAGES CXX)

include(CTest)

if(NOT DEFINED CMAKE_CXX_STANDARD)
  set(CMAKE_CXX_STANDARD 17)
endif(NOT DEFINED CMAKE_CXX_STANDARD)
set(CMAKE_POSITION_INDEPENDENT_CODE TRUE)
set(CMAKE_INTERPROCEDURAL_OPTIMIZATION FALSE)

if(CMAKE_VERSION VERSION_GREATER_EQUAL 3.24)
  cmake_policy(SET CMP0135 NEW)
endif()

find_package(Python3 REQUIRED COMPONENTS Interpreter Development.Module)
add_subdirectory(cmake/dependencies dependencies)

set(TOP_LEVEL_DIR ${CMAKE_CURRENT_LIST_DIR})
include_directories(${TOP_LEVEL_DIR} ${pybind11_INCLUDE_DIRS})

add_subdirectory(pybind11_abseil)
