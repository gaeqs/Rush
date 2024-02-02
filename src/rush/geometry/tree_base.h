//
// Created by gaelr on 29/01/2024.
//

#ifndef TREE_BASE_H
#define TREE_BASE_H

#include <array>
#include <vector>
#include <memory>

namespace rush {
    template<typename Storage>
    struct TreeContent {
        std::any bounds;
        Storage storage;
    };

    template<size_t Dimensions, typename Storage, typename Type,
        size_t MaxObjects, size_t Depth>
    class Tree {
        using ChildType = Tree<Dimensions, Storage, Type,
            MaxObjects, Depth - 1>;

        rush::AABB<Dimensions, Type> _aabb;
        std::unique_ptr<std::array<ChildType, 1 << Dimensions>> _children;
        std::vector<TreeContent<Storage>> _contents;
        bool _leaf;

        void split();

        Tree();

    public:
        explicit Tree(rush::AABB<Dimensions, Type> aabb);

        void insert(const Storage& storage, const std::any& bounds);

        [[nodiscard]]
        bool isLeaf() const;
    };
}

#include <rush/geometry/tree_impl.h>

#endif //TREE_BASE_H
