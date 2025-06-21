//
// Created by gaeqs on 20/06/25.
//

#ifndef RUSH_MESH_BASE_H
#define RUSH_MESH_BASE_H

#include <vector>

#include <rush/vector/vec.h>

namespace rush
{
    template<size_t Dimensions, typename Type, typename ExtraData>
    struct Vertex
    {
        Vec<Dimensions, Type> position;
        ExtraData extraData;
    };

    template<size_t Dimensions, typename Type>
    struct Vertex<Dimensions, Type, void>
    {
        Vec<Dimensions, Type> position;
    };

    template<size_t Dimensions, typename Type, typename ExtraData>
    struct Mesh
    {
        using LocalVertex = Vertex<Dimensions, Type, ExtraData>;

        std::vector<LocalVertex> vertices;
        std::vector<size_t> indices;

        size_t weld();

        size_t removeUnusedVertices();

        void reverseOrder(size_t verticesPerPatch);
    };

} // namespace rush

#include <rush/mesh/mesh_impl.h>

#endif // RUSH_MESH_BASE_H
