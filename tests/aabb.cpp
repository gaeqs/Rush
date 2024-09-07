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

    REQUIRE(rush::intersects(aabb, rush::Vec3f(3.0f, 5.0f, 6.0f)));
    REQUIRE(rush::intersects(aabb, rush::Vec3f(0.0f, 9.0f, 3.0f)));
    REQUIRE_FALSE(rush::intersects(aabb, rush::Vec3f(0.0f, 0.0f, 0.0f)));
    REQUIRE_FALSE(rush::intersects(rush::Vec3f(0.0f, 0.0f, 0.0f), aabb));
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

    REQUIRE(rush::intersects(aabb1, aabb2));
    REQUIRE_FALSE(rush::intersects(aabb1, aabb3));
}

TEST_CASE("AABB closest point", "[aabb]") {
    rush::Vec3f min = {1.0f, 1.0f, 1.0f};
    rush::Vec3f max = {5.0f, 5.0f, 5.0f};
    auto aabb = rush::AABB<3, float>::fromEdges(min, max);

    requireSimilar(aabb.closestPoint({0.0f, 0.0f, 0.0f}), min);
    requireSimilar(aabb.closestPoint({9.0f, 9.0f, 9.0f}), max);

    requireSimilar(aabb.closestPoint({2.0f, 9.0f, 2.0f}),
                   {2.0f, 5.0f, 2.0f});

    requireSimilar(aabb.closestPoint({2.0f, 0.0f, 2.0f}),
                   {2.0f, 1.0f, 2.0f});

    requireSimilar(aabb.closestPoint({-20.0f, 4.0f, 10.0f}),
                   {1.0f, 4.0f, 5.0f});
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


TEST_CASE("AABB - Sphere", "[aabb]") {
    rush::Vec3f min = {1.0f, 1.0f, 1.0f};
    rush::Vec3f max = {5.0f, 5.0f, 5.0f};
    auto aabb = rush::AABB<3, float>::fromEdges(min, max);

    rush::Vec3f center = {2.0f, 8.0f, 2.0f};
    float radius1 = 5.0f;
    float radius2 = 1.0f;
    rush::Sphere sphere1(center, radius1);
    rush::Sphere sphere2(center, radius2);

    REQUIRE(rush::intersects(aabb, sphere1));
    REQUIRE(rush::intersects(sphere1, aabb));

    REQUIRE_FALSE(rush::intersects(aabb, sphere2));
    REQUIRE_FALSE(rush::intersects(sphere2, aabb));
}
