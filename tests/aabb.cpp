//
// Created by gaelr on 26/01/2024.
//

#include "test_common.h"

TEST_CASE("AABB default", "[aabb]") {
    rush::AABB<3, float> aabb;
    requireSimilar(aabb.center, rush::Vec<3, float>(0));
    requireSimilar(aabb.radius, rush::Vec<3, float>(0));
}

TEST_CASE("AABB inside", "[aabb]") {
    rush::AABB<3, float> aabb(
        {3.0f, 5.0f, 6.0f},
        {4.0f, 5.0f, 4.0f}
    );

    REQUIRE(aabb.test(rush::Vec3f(3.0f, 5.0f, 6.0f)));
    REQUIRE(aabb.test(rush::Vec3f(0.0f, 9.0f, 3.0f)));
    REQUIRE_FALSE(aabb.test(rush::Vec3f(0.0f, 0.0f, 0.0f)));
}


TEST_CASE("AABB collision", "[aabb]") {
    rush::AABB<3, float> aabb1(
        {3.0f, 5.0f, 6.0f},
        {4.0f, 5.0f, 4.0f}
    );

    rush::AABB<3, float> aabb2(
        {5.0f, 10.0f, 11.0f},
        {2.0f, 2.0f, 2.0f}
    );

    rush::AABB<3, float> aabb3(
        {0.0f, 20.0f, 11.0f},
        {2.0f, 2.0f, 2.0f}
    );

    REQUIRE(aabb1.test(aabb2));
    REQUIRE_FALSE(aabb1.test(aabb3));
}


TEST_CASE("AABB from edges", "[aabb]") {
    auto aabb1 = rush::AABB<3, float>::fromEdges(
        {1.0f, 1.0f, 1.0f},
        {5.0f, 5.0f, 5.0f}
    );
    auto aabb2 = rush::AABB<3, float>::fromEdges(
        {5.0f, 5.0f, 5.0f},
        {1.0f, 1.0f, 1.0f}
    );

    requireSimilar(aabb1.center, aabb2.center);
    requireSimilar(aabb1.radius, aabb2.radius);
}
