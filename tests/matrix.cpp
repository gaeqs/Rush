#include <iostream>
#include <random>
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

const Mat4f random{8.1f, 1.3f, 9.0f, 8.3f,
                   1.2f, 9.6f, 9.1f, 2.2f,
                   6.3f, 3.0f, 0.9f, 5.4f,
                   2.7f, 1.8f, 5.4f, 9.9f};

inline void requireSimilar(float a, float b) {
    REQUIRE(std::abs(a - b) < 0.01f);
}

template<size_t C, size_t R, typename Type>
inline void requireSimilar(rush::Mat<C, R, Type> a, rush::Mat<C, R, Type> b) {
    for (int c = 0; c < C; ++c) {
        for (int r = 0; r < R; ++r) {
            requireSimilar(a(c, r), b(c, r));
        }
    }
}

TEST_CASE("Matrix creation", "[matrix]") {
    REQUIRE_NOTHROW(Mat3());
    REQUIRE_NOTHROW(Mat3(1));
    REQUIRE_NOTHROW(Mat2(Mat3(1, 2, 3, 4, 5)));

    REQUIRE(Mat3() == Mat3(0));
    REQUIRE(Mat3(1) == Mat3(1, 0, 0, 0, 1, 0, 0, 0, 1));
    REQUIRE(Mat3(1) != Mat3(1, 0, 0, 0, 0, 0, 0, 0, 1));
    REQUIRE(Mat3(100) == Mat3(100, 0, 0, 0, 100, 0, 0, 0, 100));

    REQUIRE(Mat3([](size_t c, size_t r) { return static_cast<int>(r + c * 3); })
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

TEST_CASE("Matrix - scale operations (float)", "[matrix]") {
    rush::Mat<2, 2, float> a(1.0f, 2.0f, 3.0f, 4.0f);
    float b = 2.0f;

    SECTION("Addition") {
        rush::Mat<2, 2, float> r(3.0f, 4.0f, 5.0f, 6.0f);
        requireSimilar(a + b, r);
    }

    SECTION("Subtract") {
        rush::Mat<2, 2, float> r(-1.0f, 0.0f, 1.0f, 2.0f);
        requireSimilar(a - b, r);
    }

    SECTION("Multiplication") {
        rush::Mat<2, 2, float> r(2.0f, 4.0f, 6.0f, 8.0f);
        requireSimilar(a * b, r);
    }

    SECTION("Division") {
        rush::Mat<2, 2, float> r(0.5f, 1.0f, 1.5f, 2.0f);
        requireSimilar(a / b, r);
    }
}

TEST_CASE("Matrix - scale operations (int)", "[matrix]") {
    rush::Mat<2, 2, int> a(1, 2, 3, 4);
    int b = 2;

    SECTION("Shift left") {
        rush::Mat<2, 2, int> r(1 << b, 2 << b, 3 << b, 4 << b);
        REQUIRE((a << b) == r);
    }

    SECTION("Shift right") {
        rush::Mat<2, 2, int> r(1 >> b, 2 >> b, 3 >> b, 4 >> b);
        REQUIRE((a >> b) == r);
    }

    SECTION("Shift and") {
        rush::Mat<2, 2, int> r(1 & b, 2 & b, 3 & b, 4 & b);
        REQUIRE((a & b) == r);
    }

    SECTION("Shift or") {
        rush::Mat<2, 2, int> r(1 | b, 2 | b, 3 | b, 4 | b);
        REQUIRE((a | b) == r);
    }

    SECTION("Shift xor") {
        rush::Mat<2, 2, int> r(1 ^ b, 2 ^ b, 3 ^ b, 4 ^ b);
        REQUIRE((a ^ b) == r);
    }
}

TEST_CASE("Matrix - scale operations (bool)", "[matrix]") {
    rush::Mat<2, 2, bool> a(false, false, true, true);

    SECTION("Shift and") {
        rush::Mat<2, 2, bool> t(false, false, true, true);
        rush::Mat<2, 2, bool> f(false, false, false, false);
        REQUIRE((a && true) == t);
        REQUIRE((a && false) == f);
    }

    SECTION("Shift or") {
        rush::Mat<2, 2, bool> t(true, true, true, true);
        rush::Mat<2, 2, bool> f(false, false, true, true);
        REQUIRE((a || true) == t);
        REQUIRE((a || false) == f);
    }
}

TEST_CASE("Matrix - Matrix operations", "[matrix]") {
    rush::Mat<2, 2, float> a(1.0f, 2.0f, 3.0f, 4.0f);
    rush::Mat<2, 2, float> b(2.0f, 2.0f, 3.0f, 3.0f);

    SECTION("Addition") {
        rush::Mat<2, 2, float> r(3.0f, 4.0f, 6.0f, 7.0f);
        requireSimilar(a + b, r);
    }

    SECTION("Subtract") {
        rush::Mat<2, 2, float> r(-1.0f, 0.0f, 0.0f, 1.0f);
        requireSimilar(a - b, r);
    }

    SECTION("Multiplication") {
        rush::Mat<2, 2, float> r(8.0f, 12.0f, 12.0f, 18.0f);
        requireSimilar(a * b, r);
    }
}

TEST_CASE("Matrix operations", "[matrix]") {
    rush::Mat<3, 2, int> a(1, 2, 3, 4, 5, 6);
    rush::Mat<4, 3, int> b(1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12);
    rush::Mat<4, 2, int> r(22, 28, 49, 64, 76, 100, 103, 136);
    REQUIRE(a * b == r);
}

TEST_CASE("Matrix determinant", "[matrix]") {
    requireSimilar(random.determinant(), -4002.91f);

    Mat3f nonInv([](size_t c, size_t r) {
        return static_cast<float>(r + c * 3);
    });

    REQUIRE(nonInv.determinant() == 0.0f);
}

TEST_CASE("Matrix inverse", "[matrix]") {
    Mat4f inverse = random.inverse();
    Mat4f identity = random * inverse;

    for (size_t i = 0; i < 4; i++) {
        requireSimilar(identity(i, i), 1.0f);
    }
}