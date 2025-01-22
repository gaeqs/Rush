#include <numbers>
#include <random>
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

const Mat4f randomMatrix{
    8.1f, 1.3f, 9.0f, 8.3f,
    1.2f, 9.6f, 9.1f, 2.2f,
    6.3f, 3.0f, 0.9f, 5.4f,
    2.7f, 1.8f, 5.4f, 9.9f
};

template<typename Matrix>
Matrix generateMatrix(size_t c, size_t r) {
    std::random_device os_seed;
    uint32_t seed = os_seed();
    std::mt19937 generator(seed);
    std::uniform_real_distribution valueDistribution(0.0f, 100.0f);

    auto m = Matrix();
    for (size_t x = 0; x < c; x++) {
        for (size_t y = 0; y < r; y++) {
            m.pushValue(x, y, valueDistribution(generator));
        }
    }

    return m;
}

template<typename Matrix>
Matrix generateSparseMatrix(size_t c, size_t r, float probability) {
    std::random_device os_seed;
    uint32_t seed = os_seed();
    std::mt19937 generator(seed);
    std::uniform_real_distribution valueDistribution(0.0f, 100.0f);

    auto m = Matrix();
    for (size_t x = 0; x < c; x++) {
        for (size_t y = 0; y < r; y++) {
            if (probability * 100.0f >= valueDistribution(generator)) {
                m.pushValue(x, y, valueDistribution(generator));
            }
        }
    }

    return m;
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
    requireSimilar(
        scale,
        {
            1.0f, 0.0f, 0.0f, 0.0f,
            0.0f, 2.0f, 0.0f, 0.0f,
            0.0f, 0.0f, 1.0f, 0.0f,
            0.0f, 0.0f, 0.0f, 1.0f
        }
    );
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
    auto s = Mat4f::scale({2.0f, -1.0f, 3.0f});
    auto rX = Mat4f::rotationX(1.0f);
    auto rY = Mat4f::rotationY(0.2f);
    auto rZ = Mat4f::rotationZ(2.0f);
    auto t = Mat4f::translate({10.0f, 50.0f, -20.0f});
    Mat4f m1 = t * rZ * rY * rX * s;

    Mat4f m2 = Mat4f::model({2.0f, -1.0f, 3.0f},
                            rush::Quat<float>::euler({1.0f, 0.2f, 2.0f}),
                            {10.0f, 50.0f, -20.0f});
    requireSimilar(m1, m2);
}

TEST_CASE("Sparse matrix diagonal", "[matrix]") {
    auto dense = rush::Mat4f(1.0f);
    auto sparse = rush::SparseMat4f(1.0f);
    REQUIRE(dense == sparse);
}

TEST_CASE("Sparse matrix fill", "[matrix]") {
    auto dense = rush::Mat4f([](size_t c, size_t r) {
        return c * 4 + r;
    });
    auto sparse = rush::SparseMat4f([](size_t c, size_t r) {
        return c * 4 + r;
    });
    REQUIRE(dense == sparse);
}

TEST_CASE("Sparse matrix random", "[matrix]") {
    std::random_device os_seed;
    uint32_t seed = os_seed();
    std::mt19937 generator(seed);
    std::uniform_real_distribution valueDistribution(0.0f, 100.0f);
    std::uniform_int_distribution<size_t> positionDistribution(0, 3);

    auto dense = rush::Mat4f();
    auto sparse = rush::SparseMat4f();

    for (size_t i = 0; i < 1000; i++) {
        float value = valueDistribution(generator);
        size_t column = positionDistribution(generator);
        size_t row = positionDistribution(generator);
        dense.pushValue(column, row, value);
        sparse.pushValue(column, row, value);
    }

    std::cout << dense << std::endl;
    std::cout << sparse << std::endl;
    REQUIRE(dense == sparse);
}

TEST_CASE("Sparse matrix operations", "[matrix]") {
    auto d1 = generateMatrix<rush::Mat4f>(4, 4);
    auto d2 = generateMatrix<rush::Mat4f>(4, 4);

    auto s1 = rush::SparseMat4f(d1);
    auto s2 = rush::SparseMat4f(d2);

    REQUIRE(d1 + d2 == s1 + s2);
    REQUIRE(d1 - d2 == s1 - s2);
    REQUIRE(d1 * d2 == s1 * s2);
    REQUIRE(d1.inverse() == s1.inverse());
    std::cout << d1.inverse() << std::endl;
}

TEST_CASE("Sparse matrix iterator", "[matrix]") {
    auto m = generateSparseMatrix<rush::Mat<10, 100, float, rush::MatSparseRep>>(10, 100, 0.2f);

    auto it = m.rep.sparseBegin();
    auto end = m.rep.sparseEnd();

    std::cout << m << std::endl;
    for (; it != end; ++it) {
        std::cout << it.column() << ", " << it.row() << " -> " << *it << std::endl;
        REQUIRE(*it == m(it.column(), it.row()));
    }
}


#ifdef RUSH_GLM

#include <glm/glm.hpp>

void consumer(rush::Mat4f) {}

void consumerRef(const rush::Mat4f&) {}

TEST_CASE("Rush - GLM (Matrix)", "[matrix]") {
    glm::mat4 glm(1.0f, 2.0f, 3.0f, 4.0f,
                  5.0f, 6.0f, 7.0f, 8.0f,
                  1.0f, 2.0f, 3.0f, 4.0f,
                  5.0f, 6.0f, 7.0f, 8.0f);

    rush::Mat4f rush = glm;
    glm::mat4 back = rush;

    REQUIRE(glm == back);

    consumer(back);
    consumerRef(back);
}

#endif
