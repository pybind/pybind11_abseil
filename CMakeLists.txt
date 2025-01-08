cmake_minimum_required(VERSION 3.18)
project(pybind11_abseil LANGUAGES CXX)

if(NOT DEFINED CMAKE_CXX_STANDARD)
  set(CMAKE_CXX_STANDARD 17)
endif()
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(CMAKE_POSITION_INDEPENDENT_CODE TRUE)
set(CMAKE_INTERPROCEDURAL_OPTIMIZATION FALSE)

if(CMAKE_VERSION VERSION_GREATER_EQUAL 3.24)
  cmake_policy(SET CMP0135 NEW)
endif()

# ============================================================================
# Options

option(USE_SYSTEM_ABSEIL "Force usage of system provided abseil-cpp" OFF)
option(USE_SYSTEM_PYBIND "Force usage of system provided pybind11" OFF)

# ============================================================================
# Testing
include(CTest)

# ============================================================================
# Find Python
find_package(Python3 REQUIRED COMPONENTS Interpreter Development.Module)

# ============================================================================
# Build dependencies
add_subdirectory(cmake/dependencies dependencies)

set(TOP_LEVEL_DIR ${CMAKE_CURRENT_LIST_DIR})
include_directories(${TOP_LEVEL_DIR} ${pybind11_INCLUDE_DIRS})
add_subdirectory(pybind11_abseil)
