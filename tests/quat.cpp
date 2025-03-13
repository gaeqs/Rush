#include <numbers>

#include "test_common.h"

TEST_CASE("Quaternion basics", "[quaternion]") {
    rush::Quat<double> q = rush::Quat<double>::euler({1.5, 0.2, 0.4});
    REQUIRE(q == q);
    REQUIRE(q == -(-q));
    REQUIRE(q.normalized() == q);
}

TEST_CASE("Quaternion Euler", "[quaternion]") {
    rush::Quat<double> q = rush::Quat<double>::euler({1.5, 0.2, 0.4});

    requireSimilar(q.x, 0.6502017);
    requireSimilar(q.y, 0.2063351);
    requireSimilar(q.z, 0.0779441);
    requireSimilar(q.s, 0.7270408);

    auto angles = q.euler();
    requireSimilar(angles.x(), 1.5);
    requireSimilar(angles.y(), 0.2);
    requireSimilar(angles.z(), 0.4);
    REQUIRE(angles.x() == q.pitch());
    REQUIRE(angles.y() == q.yaw());
    REQUIRE(angles.z() == q.roll());
    requireSimilar(q, rush::Quat<double>::euler(angles));
}

TEST_CASE("Quaternion rotation", "[quaternion]") {
    auto q = rush::Quat<double>::angleAxis(std::numbers::pi / 2.0,
                                           {0.0, 1.0, 0.0});

    rush::Vec<3, double> v = {1.0, 0.0, 0.0};
    requireSimilar(q * v, {0.0, 0.0, -1.0});

    auto q2 = rush::Quat<double>::angleAxis(std::numbers::pi / 2.0,
                                            {1.0, 0.0, 0.0});

    requireSimilar((q * v).squaredLength(), 1.0);

    requireSimilar(q2 * q * v, {0.0, 1.0, 0.0});
    requireSimilar(q * (q2 * v), q * q2 * v);
    requireSimilar(q * (q2 * v), q * q2 * v);

    // Quaternions doesn't have commutative property!
    requireNotSimilar(q * q2, q2 * q);
}

TEST_CASE("Quaternion normalization", "[quaternion]") {
    for (double d = 0; d < 2.0 * std::numbers::pi; d += 0.1) {
        auto q = rush::Quat<double>::angleAxis(d,
                                               {0.0, 1.0, 0.0});
        rush::Vec3d vec = {0.0, 0.0, 1.0};
        requireSimilar(vec.squaredLength(), (q * vec).squaredLength());
    }

    rush::Vec3d axis = {0.0, 1.0, 1.0};
    axis = axis.normalized();
    for (double d = 0; d < 2.0 * std::numbers::pi; d += 0.1) {
        auto q = rush::Quat<double>::angleAxis(d, axis);
        rush::Vec3d vec = {0.0, 0.0, 2.0};
        requireSimilar(vec.squaredLength(), (q * vec).squaredLength());
    }
}

TEST_CASE("Quaternion slerp", "[quaternion]") {
    auto q1 = rush::Quat<double>::angleAxis(std::numbers::pi / 2.0,
                                            {0.0, 1.0, 0.0});
    auto q2 = rush::Quat<double>::angleAxis(std::numbers::pi / 2.0,
                                            {1.0, 0.0, 0.0});

    requireSimilar(q1.slerp(q2, 0.5), {0.816496, 0.40824837, 0.40824837, 0.0});

    // Edge cases
    requireSimilar(q1.slerp(q1, 0.3), q1);
    requireSimilar(q1.slerp(-q1, 0.3), q1);

    // Invalid!
    requireSimilar(q1.slerp(q1.conjugate(), 0.5), {1.0, 0.0, 0.0, 0.0});
}

TEST_CASE("Quaternion iterator", "[quaternion]") {
    auto q1 = rush::Quat<double>::angleAxis(std::numbers::pi / 2.0,
                                            {0.0, 1.0, 0.0}); {
        // BEGIN - END
        size_t i = 0;
        auto it = q1.begin();
        while (it != q1.end()) {
            REQUIRE(*it++ == q1[i++]);
        }
        REQUIRE(i == 4);
    } {
        // CBEGIN - CEND
        size_t i = 0;
        auto it = q1.cbegin();
        while (it != q1.cend()) {
            REQUIRE(*it++ == q1[i++]);
        }
        REQUIRE(i == 4);
    } {
        // RBEGIN - REND
        size_t c = 0;
        size_t i = 3;
        auto it = q1.rbegin();
        while (it != q1.rend()) {
            REQUIRE(*it++ == q1[i--]);
            c++;
        }
        REQUIRE(c == 4);
    } {
        // CRBEGIN - CREND
        size_t c = 0;
        size_t i = 3;
        auto it = q1.crbegin();
        while (it != q1.crend()) {
            REQUIRE(*it++ == q1[i--]);
            c++;
        }
        REQUIRE(c == 4);
    } {
        // FOR LOOP
        size_t i = 0;
        for (double d: q1) {
            REQUIRE(d == q1[i++]);
        }
    }
}

TEST_CASE("Quaternion matrix", "[quaternion]") {
    auto q1 = rush::Quat<double>::euler({2.0, 1.0, 0.5});
    auto rX = rush::Mat4d::rotationX(2.0);
    auto rY = rush::Mat4d::rotationY(1.0);
    auto rZ = rush::Mat4d::rotationZ(0.5);

    auto mat1 = rZ * rY * rX;
    auto mat2 = q1.rotationMatrix4();

    requireSimilar(mat1, mat2);
}

TEST_CASE("Quarernion - euler edges", "[quaternion]") {
    constexpr float PI = std::numbers::pi_v<float>;
    auto q1 = rush::Quatf(0, 0, 0, 1);
    auto result = q1.euler();
    REQUIRE((result.x() == PI || result.y() == PI || result.z() == PI));
}

TEST_CASE("Quarernion from-to", "[quaternion]") {
    auto a = rush::Vec3f(1.0f, 0.0f, 0.0f);
    auto b = rush::Vec3f(0.0f, 0.0f, 1.0f);
    auto q1 = rush::Quatf::fromTo(a, b);
    requireSimilar(q1 * a, b);
}

TEST_CASE("Quarernion - matrix", "[quaternion]") {
    auto q1 = rush::Quatf::fromTo({-1.0f, 0.0f, 0.0f}, {0.0f, 0.5f, -2.0f});
    auto matrix = q1.rotationMatrix3();
    auto q2 = rush::Quatf::fromRotationMatrix(matrix);

    requireSimilar(q1, q2);
}

#ifdef RUSH_GLM

#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>

void consumer(rush::Quatf) {}

void consumerRef(const rush::Quatf&) {}

TEST_CASE("Rush - GLM (Quaternion)", "[quaternion]") {
    glm::quat glm(1.0f, 2.0f, 3.0f, 4.0f);

    rush::Quatf rush = glm;
    glm::quat back = rush;

    REQUIRE(glm == back);

    consumer(back);
    consumerRef(back);
}

#endif
