cmake_minimum_required(VERSION 3.30.1)
project(tatlin_test)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_VERBOSE_MAKEFILE off)
set(CMAKE_FIND_DEBUG_MODE off)

set(CMAKE_TOOLCHAIN_FILE "C:/vcpkg/scripts/buildsystems/vcpkg.cmake" CACHE STRING "")

add_subdirectory(src)
add_subdirectory(tests)