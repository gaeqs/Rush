#include <numbers>
#include <rush/matrix/mat.h>

#include "test_common.h"

using V1 = rush::Vec<1, int>;
using V2 = rush::Vec<2, int>;
using V3 = rush::Vec<3, int>;
using V4 = rush::Vec<4, int>;
using V5 = rush::Vec<5, int>;
using V4f = rush::Vec<4, float>;
using Mat2 = rush::Mat<2, 2, int>;
using Mat3 = rush::Mat<3, 3, int>;
using Mat4 = rush::Mat<4, 4, int>;
using Mat2f = rush::Mat<2, 2, float>;
using Mat3f = rush::Mat<3, 3, float>;
using Mat4f = rush::Mat<4, 4, float>;

const Mat4f randomMatrix{8.1f, 1.3f, 9.0f, 8.3f,
                         1.2f, 9.6f, 9.1f, 2.2f,
                         6.3f, 3.0f, 0.9f, 5.4f,
                         2.7f, 1.8f, 5.4f, 9.9f};

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

    Mat2 mat(0, 1, 2, 3);
    REQUIRE(Mat3(mat, 1) == Mat3(0, 1, 0, 2, 3, 0, 0, 0, 1));
    REQUIRE(rush::Mat<3, 2, int>(mat, 1) ==
            rush::Mat<3, 2, int>(0, 1, 2, 3, 0, 0));
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

TEST_CASE("Matrix - scalar operations (float)", "[matrix]") {
    rush::Mat<2, 2, float> a(1.0f, 2.0f, 3.0f, 4.0f);
    float b = 2.0f;

    SECTION("Addition") {
        rush::Mat<2, 2, float> r(3.0f, 4.0f, 5.0f, 6.0f);
        requireSimilar(a + b, r);
        requireSimilar(a + b, b + a);
    }

    SECTION("Subtract") {
        rush::Mat<2, 2, float> r(-1.0f, 0.0f, 1.0f, 2.0f);
        requireSimilar(a - b, r);
        requireSimilar(a - b, -(b - a));
    }

    SECTION("Multiplication") {
        rush::Mat<2, 2, float> r(2.0f, 4.0f, 6.0f, 8.0f);
        requireSimilar(a * b, r);
        requireSimilar(a * b, b * a);
    }

    SECTION("Division") {
        rush::Mat<2, 2, float> r(0.5f, 1.0f, 1.5f, 2.0f);
        requireSimilar(a / b, r);
    }
}

TEST_CASE("Matrix - scalar operations (int)", "[matrix]") {
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
        REQUIRE((a & b) == (b & a));
    }

    SECTION("Shift or") {
        rush::Mat<2, 2, int> r(1 | b, 2 | b, 3 | b, 4 | b);
        REQUIRE((a | b) == r);
        REQUIRE((a | b) == (b | a));
    }

    SECTION("Shift xor") {
        rush::Mat<2, 2, int> r(1 ^ b, 2 ^ b, 3 ^ b, 4 ^ b);
        REQUIRE((a ^ b) == r);
        REQUIRE((a ^ b) == (b ^ a));
    }
}

TEST_CASE("Matrix - scalar operations (bool)", "[matrix]") {
    rush::Mat<2, 2, bool> a(false, false, true, true);

    SECTION("Shift and") {
        rush::Mat<2, 2, bool> t(false, false, true, true);
        rush::Mat<2, 2, bool> f(false, false, false, false);
        REQUIRE((a && true) == t);
        REQUIRE((a && false) == f);

        REQUIRE((a && true) == (true && a));
        REQUIRE((a && false) == (false && a));
    }

    SECTION("Shift or") {
        rush::Mat<2, 2, bool> t(true, true, true, true);
        rush::Mat<2, 2, bool> f(false, false, true, true);
        REQUIRE((a || true) == t);
        REQUIRE((a || false) == f);
        REQUIRE((a || true) == (true || a));
        REQUIRE((a || false) == (false || a));
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
    requireSimilar(randomMatrix.determinant(), -4002.91f);

    Mat3f nonInv([](size_t c, size_t r) {
        return static_cast<float>(r + c * 3);
    });

    REQUIRE(nonInv.determinant() == 0.0f);
}

TEST_CASE("Matrix inverse", "[matrix]") {
    Mat4f inverse = randomMatrix.inverse();
    Mat4f identity = randomMatrix * inverse;

    for (size_t i = 0; i < 4; i++) {
        requireSimilar(identity(i, i), 1.0f);
    }
}

TEST_CASE("Matrix translation", "[matrix]") {
    V4f vec = {10.0f, 20.0f, 30.0f, 1.0f};
    Mat4f trans = Mat4f::translate({-10.0f, -5.0f, 1.0f});
    requireSimilar(trans * vec, {0.0f, 15.0f, 31.0f, 1.0f});
    requireSimilar(trans.inverse(), Mat4f::translate({10.0f, 5.0f, -1.0f}));
}

TEST_CASE("Matrix scale", "[matrix]") {
    Mat4f scale = Mat4f::scale({1.0f, 2.0f, 1.0f});
    V4f vec = {10.0f, 20.0f, 30.0f, 1.0f};
    requireSimilar(scale * vec, {10.0f, 40.0f, 30.0f, 1.0f});
    requireSimilar(scale, {1.0f, 0.0f, 0.0f, 0.0f,
                           0.0f, 2.0f, 0.0f, 0.0f,
                           0.0f, 0.0f, 1.0f, 0.0f,
                           0.0f, 0.0f, 0.0f, 1.0f});
    requireSimilar(scale.inverse(), Mat4f::scale({1.0f, 0.5f, 1.0f}));
}

TEST_CASE("Matrix rotation X", "[matrix]") {
    V4f vec = {0.0f, 1.0f, 0.0f, 1.0f};
    Mat4f rotX = Mat4f::rotationX(std::numbers::pi_v<float> / 2.0f);
    requireSimilar(rotX * vec, {0.0f, 0.0f, 1.0f, 1.0f});
    requireSimilar(rotX.inverse(), rotX.transpose());
    requireSimilar(rotX.inverse(),
                   Mat4f::rotationX(std::numbers::pi_v<float> / -2.0f));
    requireSimilar(rotX * rotX * rotX * rotX * vec, vec);
    requireSimilar((rotX * rotX) * (rotX * rotX * vec), vec);
}

TEST_CASE("Matrix rotation Y", "[matrix]") {
    V4f vec = {1.0f, 0.0f, 0.0f, 1.0f};
    Mat4f rotY = Mat4f::rotationY(std::numbers::pi_v<float> / 2.0f);
    requireSimilar(rotY * vec, {0.0f, 0.0f, -1.0f, 1.0f});
    requireSimilar(rotY.inverse(), rotY.transpose());
    requireSimilar(rotY.inverse(),
                   Mat4f::rotationY(std::numbers::pi_v<float> / -2.0f));
    requireSimilar(rotY * rotY * rotY * rotY * vec, vec);
    requireSimilar((rotY * rotY) * (rotY * rotY * vec), vec);
}

TEST_CASE("Matrix rotation Z", "[matrix]") {
    V4f vec = {1.0f, 0.0f, 0.0f, 1.0f};
    Mat4f rotZ = Mat4f::rotationZ(std::numbers::pi_v<float> / 2.0f);
    requireSimilar(rotZ * vec, {0.0f, 1.0f, 0.0f, 1.0f});
    requireSimilar(rotZ.inverse(), rotZ.transpose());
    requireSimilar(rotZ.inverse(),
                   Mat4f::rotationZ(std::numbers::pi_v<float> / -2.0f));
    requireSimilar(rotZ * rotZ * rotZ * rotZ * vec, vec);
    requireSimilar((rotZ * rotZ) * (rotZ * rotZ * vec), vec);
}

TEST_CASE("Matrix model", "[matrix]") {
    Mat4f m = Mat4f::model({1.0f, 1.0f, 1.0f},
                           rush::Quat<float>(),
                           {0.0f, 1.0f, 0.0f});
    std::cout << m << std::endl;
}