#include <any>
#include <random>
#include <vector>

#include "test_common.h"

TEST_CASE("Tree benchmark", "[!benchmark][tree]") {
    constexpr size_t SIZE = 100;

    std::random_device os_seed;
    uint32_t seed = os_seed();
    std::mt19937 generator(seed);
    std::uniform_real_distribution d(-10.0f, 10.0f);

    BENCHMARK_ADVANCED("Add 100")(Catch::Benchmark::Chronometer meter) {
        rush::Tree<int, 3, float, 4, 4> tree({
            {0.0f, 0.0f, 0.0f}, {10.0f, 10.0f, 10.0f}
        });

        std::vector<rush::AABB<3, float>> vector;

        for (size_t i = 0; i < SIZE; ++i) {
            vector.push_back(rush::AABB<3, float>::fromEdges(
                {d(generator), d(generator), d(generator)},
                {d(generator), d(generator), d(generator)}
            ));
        }

        meter.measure([&vector, &tree] {
            for (size_t i = 0; i < 100; ++i) {
                tree.insert(i, vector[i]);
            }
        });
    };


    BENCHMARK_ADVANCED("Add remove 100")(Catch::Benchmark::Chronometer meter) {
        rush::Tree<int, 3, float, 4, 4> tree({
            {0.0f, 0.0f, 0.0f}, {10.0f, 10.0f, 10.0f}
        });

        std::vector<rush::AABB<3, float>> vector;

        for (size_t i = 0; i < SIZE; ++i) {
            vector.push_back(rush::AABB<3, float>::fromEdges(
                {d(generator), d(generator), d(generator)},
                {d(generator), d(generator), d(generator)}
            ));
        }

        for (size_t i = 0; i < SIZE; ++i) {
            tree.insert(i, vector[i]);
        }

        meter.measure([&vector, &tree, SIZE] {
            for (size_t i = 0; i < SIZE; ++i) {
                tree.remove(i);
            }
        });
    };
}
