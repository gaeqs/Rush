//
// Created by gaeqs on 20/06/25.
//

#include "test_common.h"

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