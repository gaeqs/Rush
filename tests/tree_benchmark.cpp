#include <any>
#include <random>
#include <vector>

#include "test_common.h"

TEST_CASE("Tree benchmark add", "[!benchmark][tree]") {
    constexpr size_t SIZE = 100;

    std::random_device os_seed;
    uint32_t seed = os_seed();
    std::mt19937 generator(seed);
    std::uniform_real_distribution d(-10.0f, 10.0f);

    BENCHMARK_ADVANCED("Add 100")(Catch::Benchmark::Chronometer meter) {
        rush::Tree<int, rush::AABB<3, float>, 3, float, 4, 4, 100> tree({
            {-10.0f, -10.0f, -10.0f}, {10.0f, 10.0f, 10.0f}
        });

        std::vector<rush::AABB<3, float> > vector;

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
}

TEST_CASE("Tree benchmark remove", "[!benchmark][tree]") {
    constexpr size_t SIZE = 100;

    std::random_device os_seed;
    uint32_t seed = os_seed();
    std::mt19937 generator(seed);
    std::uniform_real_distribution d(-10.0f, 10.0f);

    BENCHMARK_ADVANCED("Add remove 100")(Catch::Benchmark::Chronometer meter) {
        rush::Tree<int, rush::AABB<3, float>, 3, float, 4, 4, 100> tree({
            {-10.0f, -10.0f, -10.0f}, {10.0f, 10.0f, 10.0f}
        });

        std::vector<rush::AABB<3, float> > vector;

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

TEST_CASE("Collision detection benchmark", "[!benchmark][tree]") {
    constexpr size_t SIZE = 1000;
    using Tree = rush::Tree<int, rush::AABB<3, float>, 3, float, 20, 4, 20>;

    std::random_device os_seed;
    uint32_t seed = os_seed();
    std::mt19937 generator(seed);
    std::uniform_real_distribution d(-10.0f, 10.0f);

    std::vector<rush::AABB<3, float> > vector;

    for (size_t i = 0; i < SIZE; ++i) {
        vector.push_back(rush::AABB<3, float>::fromEdges(
            {d(generator), d(generator), d(generator)},
            {0.1f, 0.1f, 0.1f}
        ));
    }

    BENCHMARK("Direct detection") {
        int count = 0;
        for (size_t i = 0; i < vector.size(); ++i) {
            for (size_t j = i + 1; j < vector.size(); ++j) {
                auto *a = &vector[i];
                auto *b = &vector[j];
                if (intersects(*a, *b)) {
                    ++count;
                }
            }
        }
        return count;
    };

    BENCHMARK("Tree") {
        Tree tree({{-10.0f, -10.0f, -10.0f}, {10.0f, 10.0f, 10.0f}});

        for (size_t i = 0; i < SIZE; ++i) {
            tree.insert(i, vector[i]);
        }

        int count = 0;
        for (auto &set: tree) {
            for (size_t i = 0; i < set.size(); ++i) {
                for (size_t j = i + 1; j < set.size(); ++j) {
                    auto *a = &set[i];
                    auto *b = &set[j];
                    if (i != j) {
                        if (rush::intersects(a->bounds, b->bounds)) {
                            ++count;
                        }
                    }
                }
            }
        }

        return count;
    };

    Tree tree({{-10.0f, -10.0f, -10.0f}, {10.0f, 10.0f, 10.0f}});

    for (size_t i = 0; i < SIZE; ++i) {
        tree.insert(i, vector[i]);
    }

    BENCHMARK("Tree already populated") {
        int count = 0;
        for (auto &set: tree) {
            for (size_t i = 0; i < set.size(); ++i) {
                for (size_t j = i + 1; j < set.size(); ++j) {
                    auto *a = &set[i];
                    auto *b = &set[j];
                    if (rush::intersects(a->bounds, b->bounds)) {
                        ++count;
                    }
                }
            }
        }

        return count;
    };
}

TEST_CASE("Collision check benchmark", "[!benchmark][tree]") {
    constexpr size_t SIZE = 100000;

    using Tree = rush::Tree<int, rush::AABB<3, float>, 3, float, 10, 3, 200>;

    std::random_device os_seed;
    uint32_t seed = os_seed();
    std::mt19937 generator(seed);
    std::uniform_real_distribution d(-10.0f, 10.0f);

    std::vector<rush::AABB<3, float> > vector;

    for (size_t i = 0; i < SIZE; ++i) {
        vector.push_back(rush::AABB<3, float>::fromEdges(
                {d(generator), d(generator), d(generator)},
                {0.1f, 0.1f, 0.1f}
        ));
    }

    Tree tree({{-10.0f, -10.0f, -10.0f}, {10.0f, 10.0f, 10.0f}});

    for (size_t i = 0; i < SIZE; ++i) {
        tree.insert(i, vector[i]);
    }

    BENCHMARK("Direct collision check") {
        auto box = rush::AABB<3, float>::fromEdges(
                {d(generator), d(generator), d(generator)},
                {0.1f, 0.1f, 0.1f}
                );
        int count = 0;
        for (const auto& i : vector) {
            if(rush::intersects(i, box)) {
                ++count;
            }
        }
        return count;
    };

    BENCHMARK("Tree collision check") {
        auto box = rush::AABB<3, float>::fromEdges(
                {d(generator), d(generator),
                 d(generator)},
                {0.1f, 0.1f, 0.1f}
        );

        auto it = tree.begin(box);
        auto end = tree.end();

        int count = 0;
        while (it != end) {
            for (const auto& entry: *it) {
                if (rush::intersects(
                        entry.bounds, box)) {
                    ++count;
                }
            }
            ++it;
        }

        return count;
    };

}
