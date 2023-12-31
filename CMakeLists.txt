cmake_minimum_required(VERSION 3.25)

include( CPack )

project(rush VERSION 0.1 LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 20)

find_package(glm) # OPTIONAL.

add_library(rush INTERFACE)
target_include_directories(rush INTERFACE
        $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/src>
        $<INSTALL_INTERFACE:src>)

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