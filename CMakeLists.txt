# SPDX-FileCopyrightText: 2026 Vovoid Media Technologies AB
#
# SPDX-License-Identifier: MIT
#
cmake_minimum_required(VERSION 3.21)
project(vzx_malloc_vector_example LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 23)

option(VZX_ASTREE "Set when running CMake to prepare for Astrée tool" OFF)

add_executable(test_malloc_vector test_malloc_vector.cpp)
add_test(NAME test_malloc_vector COMMAND test_malloc_vector)
target_include_directories(test_malloc_vector PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
if (${VZX_ASTREE})
  target_compile_options(test_malloc_vector PRIVATE -DVZX_ASTREE=1)
endif()
add_executable(test_new_vector test_new_vector.cpp)
add_test(NAME test_new_vector COMMAND test_new_vector)
target_include_directories(test_new_vector PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
if (${VZX_ASTREE})
  target_compile_options(test_new_vector PRIVATE -DVZX_ASTREE=1)
endif()

enable_testing()
