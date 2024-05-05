//
// Created by gaelr on 29/01/2024.
//

#include <any>
#include <random>

#include "test_common.h"


void a(std::any any) {
    std::cout << any.type().name() << std::endl;
}

TEST_CASE("Test", "[tree]") {
    std::cout << "TEST!" << std::endl;
}

TEST_CASE("Tree basics", "[tree]") {
    constexpr size_t SIZE = 100;

    std::random_device os_seed;
    uint32_t seed = os_seed();
    std::mt19937 g(seed);
    std::uniform_real_distribution d(-10.0f, 10.0f);

    auto rVec = [&g, &d]() {
        return rush::Vec3f(d(g), d(g), d(g));
    };

    rush::AABB<3, float> aabb = rush::AABB<3, float>::fromEdges(
        rush::Vec3f(-10.0f), rush::Vec3f(10.0f));

    rush::Tree<int, 3, float, 10, 4> tree(aabb);

    for (size_t i = 0; i < SIZE; ++i) {
        auto center = rVec();
        auto radius = rush::Vec3f(0.1f, 0.1f, 0.1f);
        auto box = rush::AABB(center, radius);
        tree.insert(i, box);
    }

    auto it = tree.begin();
    auto end = tree.end();

    while (it != end) {
        auto& contents = *it;
        auto bounds = it.getTree()->getBounds();

        REQUIRE(std::all_of(
            contents.begin(),
            contents.end(),
            [&bounds](const rush::TreeContent<int>& a) {
            return rush::intersectsAny<3, float>(a.bounds, bounds);
            }
        ));
        ++it;
    }
}
