//
// Created by gaeqs on 20/06/25.
//

#include "test_common.h"

#include <random>

TEST_CASE("Mesh", "[mesh]")
{
    rush::Mesh<3, float, int> mesh;
    mesh.vertices.push_back(rush::Vertex<3, float, int>({0.0f, 1.0f, 2.0f}, 0));
    mesh.vertices.push_back(rush::Vertex<3, float, int>({2.0f, 3.0f, 8.0f}, 0));

    mesh.indices = {1, 2, 0, 1, 0, 0, 2};

    for (size_t& index : mesh.indices) {
        std::cout << index << " ";
    }
    std::cout << std::endl;

    for (auto& vertex : mesh.vertices) {
        std::cout << vertex.position << " ";
    }
    std::cout << std::endl;

    std::cout << mesh.removeUnusedVertices() << std::endl;

    for (size_t& index : mesh.indices) {
        std::cout << index << " ";
    }
    std::cout << std::endl;

    for (auto& vertex : mesh.vertices) {
        std::cout << vertex.position << " ";
    }
    std::cout << std::endl;

    mesh.reverseOrder(3);

    for (size_t& index : mesh.indices) {
        std::cout << index << " ";
    }
    std::cout << std::endl;

    for (auto& vertex : mesh.vertices) {
        std::cout << vertex.position << " ";
    }
    std::cout << std::endl;
}

TEST_CASE("Mesh weld benchmark", "[!benchmark][mesh]")
{
    constexpr size_t SIZE = 10000;

    std::random_device os_seed;
    uint32_t seed = os_seed();
    std::mt19937 generator(seed);
    std::uniform_real_distribution d(-10.0f, 10.0f);

    BENCHMARK_ADVANCED("Add")(Catch::Benchmark::Chronometer meter)
    {
        rush::Mesh<3, float, void> mesh;

        for (size_t i = 0; i < SIZE; ++i) {
            mesh.vertices.push_back(rush::Vertex<3, float, void>({
                {d(generator), d(generator), d(generator)}
            }));
        }

        meter.measure([&mesh] { mesh.weld(); });
    };
}