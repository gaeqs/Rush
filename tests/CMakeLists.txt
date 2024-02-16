find_package(Catch2 REQUIRED)

include(CTest)
include(Catch)

project(rush-tests)
set(CMAKE_CXX_STANDARD 20)

add_executable(rush-tests
        vector.cpp
        matrix.cpp
        vector_benchmark.cpp
        matrix_benchmark.cpp
        quat.cpp
        bezier.cpp
        scalar_math.cpp
        test_common.h
        aabb.cpp
        tree.cpp
        tree_benchmark.cpp)
target_link_libraries(rush-tests PUBLIC rush Catch2::Catch2WithMain)

catch_discover_tests(rush-tests)