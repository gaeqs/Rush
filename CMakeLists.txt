cmake_minimum_required(VERSION 3.25)

project(rush)
set(CMAKE_CXX_STANDARD 20)

find_package(glm) # OPTIONAL.

add_library(rush INTERFACE)
target_include_directories(rush INTERFACE src)

if (glm_FOUND)
    target_compile_definitions(rush INTERFACE RUSH_GLM)
endif ()

add_subdirectory(tests)