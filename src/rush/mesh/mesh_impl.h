//
// Created by gaeqs on 20/06/25.
//

#ifndef MESH_IMPL_H
#define MESH_IMPL_H

#include <rush/geometry/static_tree_base.h>

namespace rush
{
    template<size_t Dimensions, typename Type, typename ExtraData>
    size_t Mesh<Dimensions, Type, ExtraData>::weld()
    {
        constexpr size_t MAX = std::numeric_limits<size_t>::max();

        if (vertices.empty()) {
            return 0;
        }

        std::vector<TreeContent<std::pair<size_t, LocalVertex*>, Vec<Dimensions, Type>>> indexed;
        indexed.reserve(vertices.size());

        auto min = vertices[0].position;
        auto max = vertices[0].position;

        for (size_t i = 0; i < vertices.size(); i++) {
            indexed.emplace_back(vertices[i].position, std::make_pair(i, &vertices[i]));
            min = rush::min(min, vertices[i].position);
            max = rush::max(max, vertices[i].position);
        }

        auto aabb = AABB<Dimensions, Type>::fromEdges(min, max);
        StaticTree<std::pair<size_t, LocalVertex*>, Vec<Dimensions, Type>, Dimensions, Type, 10, 5> tree(aabb, indexed);

        Sphere<Dimensions, Type> sphere({}, 0.001f);

        std::vector<size_t> verticesToWeld(vertices.size(), MAX);
        size_t welded = 0;

        for (size_t i = 0; i < vertices.size(); i++) {
            if (verticesToWeld[i] != MAX) {
                continue;
            }

            sphere.center = vertices[i].position;

            tree.forEachIntersection(
                sphere, [&](const TreeContent<std::pair<size_t, LocalVertex*>, Vec<Dimensions, Type>>& content) {
                    if (content.storage.first != i) {
                        if constexpr (std::equality_comparable<ExtraData>) {
                            if (content.storage.second->extraData != vertices[i].extraData) {
                                return;
                            }
                        }

                        verticesToWeld[content.storage.first] = i;
                        ++welded;
                    }
                });
        }

        if (welded == 0) {
            return 0;
        }

        for (size_t& index : indices) {
            if (const size_t to = verticesToWeld[index]; to != MAX) {
                index = to;
            }
        }

        return welded;
    }

    template<size_t Dimensions, typename Type, typename ExtraData>
    size_t Mesh<Dimensions, Type, ExtraData>::removeUnusedVertices()
    {
        constexpr size_t MAX = std::numeric_limits<size_t>::max();

        std::vector<size_t> remap(vertices.size(), MAX);

        size_t nextIdx = 0;
        for (size_t& idx : indices) {
            if (idx >= vertices.size()) {
                continue;
            }
            if (remap[idx] == MAX) {
                remap[idx] = nextIdx++;
            }
            idx = remap[idx];
        }

        std::vector<LocalVertex> result(nextIdx, LocalVertex{});

        for (size_t oldIdx = 0; oldIdx < vertices.size(); ++oldIdx) {
            size_t newIdx = remap[oldIdx];
            if (newIdx != MAX) {
                result[newIdx] = vertices[oldIdx];
            }
        }

        size_t amount = vertices.size() - nextIdx;
        vertices = std::move(result);
        return amount;
    }

    template<size_t Dimensions, typename Type, typename ExtraData>
    void Mesh<Dimensions, Type, ExtraData>::reverseOrder(size_t verticesPerPatch)
    {
        if (verticesPerPatch < 2) {
            return;
        }

        for (size_t i = 0; i + verticesPerPatch <= indices.size(); i += verticesPerPatch) {
            for (size_t n = 0; n < verticesPerPatch >> 1; ++n) {
                std::swap(indices[i + n], indices[i + verticesPerPatch - n - 1]);
            }
        }
    }
} // namespace rush

#endif // MESH_IMPL_H
