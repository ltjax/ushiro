cmake_minimum_required(VERSION 3.0)
project(ushiro)

set(CMAKE_CXX_STANDARD 14)

# Get dependencies
include(${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)
conan_basic_setup(TARGETS)

# Go down the tree
add_subdirectory(source)
add_subdirectory(tests)
