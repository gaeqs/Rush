//
// Created by gaelrialcostas on 3/06/24.
//

#ifndef RUSH_DYNAMIC_TREE_BASE_H
#define RUSH_DYNAMIC_TREE_BASE_H

#include <rush/geometry/tree_base.h>

namespace rush {

    enum class TreeInsertResult {
        ADDED, REMOVED, UPDATED, NOTHING
    };

    template<typename Storage, typename Bounds,
            size_t Dimensions, typename Type,
            size_t MaxObjects, size_t Depth,
            size_t MaxSubtrees, bool Root = true>
    class DynamicTree
            : public AbstractTree<AABB<Dimensions, Type>, Storage, Bounds> {

        using ChildType = std::conditional_t<(Depth > 0),
                DynamicTree<Storage, Bounds, Dimensions, Type, MaxObjects,
                        Depth - 1, MaxSubtrees, false>,
                void*>;
        using Children = std::array<ChildType, 1 << Dimensions>;

        friend class DynamicTree<Storage, Bounds, Dimensions, Type, MaxObjects,
                Depth + 1, MaxSubtrees, true>;

        friend class DynamicTree<Storage, Bounds, Dimensions, Type, MaxObjects,
                Depth + 1, MaxSubtrees, false>;

        std::conditional_t<Root,
                Pool<MaxSubtrees, sizeof(Children)>,
                Pool<MaxSubtrees, sizeof(Children)>*> _pool;

        AABB<Dimensions, Type> _aabb;
        Children* _children;
        std::vector<TreeContent<Storage, Bounds>> _contents;
        size_t _size;
        bool _leaf;

        void split();

    public:

        explicit DynamicTree() requires (!Root);

        explicit DynamicTree(AABB<Dimensions, Type> aabb) requires(Root);

        ~DynamicTree() override;

        [[nodiscard]] const std::vector<TreeContent<Storage, Bounds>>&
        getStorage() const override;

        [[nodiscard]] std::vector<TreeContent<Storage, Bounds>>&
        getStorage() override;

        std::vector<AbstractTree<AABB<Dimensions, Type>, Storage, Bounds>*>
        getChildren() const override;

        const AABB<Dimensions, Type>& getBounds() const override;

        [[nodiscard]] size_t size() const override;

        TreeInsertResult insert(const Storage& storage, const Bounds& bounds);

        bool remove(const Storage& storage);

        bool contains(const Storage& storage);

        [[nodiscard]]
        bool isLeaf() const;

        TreeIterator<AABB<Dimensions, Type>, Storage, Bounds> begin();

        template<typename FilterBounds>
        TreeIterator<AABB<Dimensions, Type>, Storage, Bounds, FilterBounds>
        begin(const FilterBounds& bounds);

        TreeIterator<AABB<Dimensions, Type>, Storage, Bounds> end();
    };


}

#include <rush/geometry/dynamic_tree_impl.h>

#endif //RUSH_DYNAMIC_TREE_BASE_H
