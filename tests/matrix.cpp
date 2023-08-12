#include <iostream>
#include <unordered_set>

#include <catch2/catch_test_macros.hpp>
#include <rush/matrix/mat.h>

using V1 = rush::Vec<1, int>;
using V2 = rush::Vec<2, int>;
using V3 = rush::Vec<3, int>;
using V5 = rush::Vec<5, int>;
using Mat2 = rush::Mat<2, 2, int>;
using Mat3 = rush::Mat<3, 3, int>;
using Mat4 = rush::Mat<4, 4, int>;
using Mat2f = rush::Mat<2, 2, float>;
using Mat3f = rush::Mat<3, 3, float>;
using Mat4f = rush::Mat<4, 4, float>;

TEST_CASE("Matrix creation", "[matrix]") {
    REQUIRE_NOTHROW(Mat3());
    REQUIRE_NOTHROW(Mat3(1));
    REQUIRE_NOTHROW(Mat2(Mat3(1, 2, 3, 4, 5)));

    REQUIRE(Mat3() == Mat3(0));
    REQUIRE(Mat3(1) == Mat3(1, 0, 0, 0, 1, 0, 0, 0, 1));
    REQUIRE(Mat3(1) != Mat3(1, 0, 0, 0, 0, 0, 0, 0, 1));
    REQUIRE(Mat3(100) == Mat3(100, 0, 0, 0, 100, 0, 0, 0, 100));

    REQUIRE(Mat3([](size_t c, size_t r) { return r + c * 3; })
            == Mat3(0, 1, 2, 3, 4, 5, 6, 7, 8));
}

TEST_CASE("Matrix accessors", "[matrix]") {
    Mat3 m = Mat3(1, 2, 3,
                  4, 5, 6,
                  7, 8, 9);
    REQUIRE(m[0] == V3(1, 2, 3));
}

TEST_CASE("Matrix multiplication", "[matrix]") {
    rush::Mat<3, 2, int> a(1, 2, 3, 4, 5, 6);
    rush::Mat<4, 3, int> b(1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12);
    rush::Mat<4, 2, int> r(22, 28, 49, 64, 76, 100, 103, 136);
    REQUIRE(a * b == r);
}

TEST_CASE("Matrix determinant", "[matrix]") {
    Mat4f a([](size_t c, size_t r) { return rand() % 10; });

    //REQUIRE(a * a.inverse() == Mat4f(1.0f));

    std::cout << std::fixed;
    std::cout << a << std::endl;
    std::cout << " - " << std::endl;
    std::cout << a.transpose() << std::endl;
}