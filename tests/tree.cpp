//
// Created by gaelr on 29/01/2024.
//

#include <any>
#include <random>

#include "test_common.h"


void a(std::any any) {
    std::cout << any.type().raw_name() << std::endl;
}

TEST_CASE("Tree basics", "[tree]") {
    std::random_device os_seed;
    uint32_t seed = os_seed();
    std::mt19937 generator(seed);
    std::uniform_real_distribution<float> distribute(-10.0f, 10.0f);


    for (size_t i = 0; i < 100; ++i) {
        std::cout << distribute(generator) << std::endl;
    }

    rush::AABB<3, float> aabb = rush::AABB<3, float>::fromEdges(
        {1.0f, 1.0f, 1.0f}, {5.0f, 5.0f, 5.0f});
    rush::Sphere<3, float> sphere({7.0f, 5.0f, 5.0f}, 5.0f);

    rush::Tree<int, 3, float, 4, 4> tree(aabb);
    tree.insert(0, aabb);
    tree.insert(1, sphere);

    REQUIRE(tree.size() == 2);
}
