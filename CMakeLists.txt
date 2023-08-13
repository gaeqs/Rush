cmake_minimum_required(VERSION 3.25)

project(rush)
set(CMAKE_CXX_STANDARD 20)

find_package(glm REQUIRED)

set(RUSH_HEADERS
        src/rush/vector/vec_base.h
        src/rush/vector/vec.h
        src/rush/concepts.h
        src/rush/vector/vec_extra.h
        src/rush/vector/vec_ref.h
        src/rush/matrix/mat.h
        src/rush/matrix/mat_base.h
        src/rush/vector/vec_impl.h
        src/rush/matrix/mat_impl.h
        src/rush/rush.h
        src/rush/quaternion/quat.h
        src/rush/quaternion/quat_base.h
        src/rush/quaternion/quat_impl.h
        src/rush/vector/vec_math.h
        src/rush/algorithm.h
)

add_library(rush INTERFACE ${RUSH_HEADERS})
include_directories(src)
target_link_libraries(rush INTERFACE glm)

add_subdirectory(tests)
