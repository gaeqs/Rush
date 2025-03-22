cmake_minimum_required(VERSION 3.25)

include(CPack)
include(CheckCXXCompilerFlag)

project(rush VERSION 0.1 LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 20)

option(RUSH_BUILD_SHARED "Build Rush as a shared library" OFF)

find_package(glm) # OPTIONAL.

add_library(rush INTERFACE)
target_include_directories(rush INTERFACE
        $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/src>
        $<INSTALL_INTERFACE:>)
target_compile_features(rush INTERFACE cxx_std_20)

if (CMAKE_CXX_COMPILER_ID STREQUAL "GNU" OR CMAKE_CXX_COMPILER_ID MATCHES "Clang")
    check_cxx_compiler_flag("-mavx2" ARCH_SUPPORT_AVX2)
    if (ARCH_SUPPORT_AVX2)
        target_compile_options(rush INTERFACE -mavx2)
    endif ()
elseif (MSVC)
    check_cxx_compiler_flag("/arch:AVX2" ARCH_SUPPORT_AVX2)
    if (ARCH_SUPPORT_AVX2)
        target_compile_options(rush INTERFACE /arch:AVX2)
    endif ()
endif ()

if (glm_FOUND)
    target_compile_definitions(rush INTERFACE RUSH_GLM)
endif ()


install(TARGETS rush
        EXPORT rushTargets
        ARCHIVE DESTINATION lib
        LIBRARY DESTINATION lib
        RUNTIME DESTINATION bin
        INCLUDES DESTINATION include/rush
)

install(DIRECTORY ${PROJECT_SOURCE_DIR}/src/
        DESTINATION include
        FILES_MATCHING PATTERN "*.h*"
)

install(EXPORT rushTargets
        FILE RushTargets.cmake
        NAMESPACE rush::
        DESTINATION lib/cmake/rush
)
