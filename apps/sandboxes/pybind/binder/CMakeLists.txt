cmake_minimum_required(VERSION 3.12)
project(add_function)

set(CMAKE_CXX_STANDARD 11)

# Find pybind11 package
find_package(pybind11 REQUIRED)

add_library(libmath MODULE bindings.cpp)
target_link_libraries(libmath PRIVATE pybind11::module)