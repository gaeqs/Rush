cmake_minimum_required(VERSION 3.25)

project(rush)
set(CMAKE_CXX_STANDARD 20)

add_library(rush INTERFACE)
target_include_directories(rush INTERFACE src)

add_subdirectory(tests)