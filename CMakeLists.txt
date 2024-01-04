cmake_minimum_required(VERSION 3.25)

include(${CMAKE_CURRENT_SOURCE_DIR}/cmake/prelude.cmake)

project(magicube VERSION 0.1.0)

add_subdirectory(${magicube_root}/libs)
add_subdirectory(${magicube_root}/code)

