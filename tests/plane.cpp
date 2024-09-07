//
// Created by gaeqs on 5/7/24.
//

#include "test_common.h"

TEST_CASE("Plane default", "[plane]") {
    rush::Plane plane({1.0f, 0.0f, 0.0f}, 1.0f);
    REQUIRE(plane.isNormalized());
    requireSimilar(plane.normal, {1.0f, 0.0f, 0.0f});
    requireSimilar(plane.distanceToPoint({2.0f, 0.0f, 0.0f}), 1.0f);
    requireSimilar(plane.distanceToPoint({2.0f, 0.0f, 0.0f}), 1.0f);
    requireSimilar(plane.distanceToPoint({0.0f, 1.0f, 0.0f}), -1.0f);
}

TEST_CASE("Plane normalization", "[plane]") {
    rush::Plane plane({2.0f, 0.0f, 0.0f}, 1.0f);
    rush::Plane normalized = plane.normalized();
    REQUIRE_FALSE(plane.isNormalized());
    REQUIRE(normalized.isNormalized());
}
