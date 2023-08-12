find_package(Catch2 3 REQUIRED)

include(CTest)
include(Catch)

project(rush-tests)
set(CMAKE_CXX_STANDARD 20)

add_executable(rush-tests vector.cpp matrix.cpp)
target_link_libraries(rush-tests PRIVATE rush Catch2::Catch2WithMain)

catch_discover_tests(rush-tests)