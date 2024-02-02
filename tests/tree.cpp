//
// Created by gaelr on 29/01/2024.
//

#include <any>

#include "test_common.h"


void a(std::any any) {
    std::cout << any.type().raw_name() << std::endl;
}

TEST_CASE("Tree basics", "[tree]") {
    rush::AABB<3, float> aabb = rush::AABB<3, float>::fromEdges(
        {1.0f, 1.0f, 1.0f}, {5.0f, 5.0f, 5.0f});
    rush::Sphere<3, float> sphere({7.0f, 5.0f, 5.0f}, 5.0f);

    rush::Tree<3, int, float, 4, 4> tree(aabb);

    REQUIRE(rush::intersectsAny<3, float>(aabb, sphere));
}
