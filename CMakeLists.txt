cmake_minimum_required(VERSION 3.25)

include(CPack)
include(CheckCXXCompilerFlag)

project(rush VERSION 0.1 LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 20)

find_package(glm) # OPTIONAL.

add_library(rush INTERFACE
        src/rush/geometry/dynamic_tree_impl.h
        src/rush/geometry/static_tree_base.h)
target_include_directories(rush INTERFACE
        $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/src>
        $<INSTALL_INTERFACE:src>)

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
        LIBRARY DESTINATION lib
        ARCHIVE DESTINATION lib
        RUNTIME DESTINATION bin
        INCLUDES DESTINATION include
)

install(DIRECTORY src/rush DESTINATION include)

install(EXPORT rushTargets
        FILE rushTargets.cmake
        NAMESPACE rush::
        DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/rush
)

include(CMakePackageConfigHelpers)
configure_package_config_file(${PROJECT_SOURCE_DIR}/cmake/RushConfig.cmake.in
        ${CMAKE_CURRENT_BINARY_DIR}/rushConfig.cmake
        INSTALL_DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/rush
        NO_SET_AND_CHECK_MACRO
        NO_CHECK_REQUIRED_COMPONENTS_MACRO
)

write_basic_package_version_file(
        ${CMAKE_CURRENT_BINARY_DIR}/rushConfigVersion.cmake
        VERSION ${PROJECT_VERSION}
        COMPATIBILITY AnyNewerVersion
)

install(FILES
        ${CMAKE_CURRENT_BINARY_DIR}/rushConfig.cmake
        ${CMAKE_CURRENT_BINARY_DIR}/rushConfigVersion.cmake
        DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/rush
)

add_subdirectory(tests)