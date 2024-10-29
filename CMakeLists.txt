cmake_minimum_required(VERSION 3.0)
project(ushiro)

set(CMAKE_CXX_STANDARD 23)

# Get dependencies
find_package(Catch2 CONFIG REQUIRED)

# Go down the tree
add_subdirectory(source)
add_subdirectory(tests)
