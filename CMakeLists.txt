cmake_minimum_required(VERSION 3.25)

include(CPack)

project(rush VERSION 0.1 LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 20)

option(RUSH_BUILD_SHARED "Build Rush as a shared library" OFF)

find_package(glm) # OPTIONAL.

add_library(rush INTERFACE)
target_include_directories(rush INTERFACE $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/src> $<INSTALL_INTERFACE:src>)
target_compile_features(rush INTERFACE cxx_std_20)

if (glm_FOUND)
    target_compile_definitions(rush INTERFACE RUSH_GLM)
endif ()

# Instalar la librería para que otros la usen con find_package()
include(GNUInstallDirs)
install(TARGETS rush EXPORT rushExport)
install(DIRECTORY src/ DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})

install(EXPORT rushExport
        FILE RushConfig.cmake
        NAMESPACE rush::rush
        DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/rush
)

add_subdirectory(tests)