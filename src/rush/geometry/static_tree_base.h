//
// Created by gaelrialcostas on 3/06/24.
//

#ifndef RUSH_STATIC_TREE_BASE_H
#define RUSH_STATIC_TREE_BASE_H

#include <rush/geometry/tree_base.h>
#include <rush/allocator/pool.h>
#include <unordered_set>


namespace rush {

    template<typename Storage, typename Bounds,
            size_t Dimensions, typename Type>
    class StaticTreeLeaf {
        AABB<Dimensions, Type> _aabb;
        TreeContent<Storage, Bounds>* _elements;
        size_t _size;

    public:

        StaticTreeLeaf() = default;

        explicit StaticTreeLeaf(const AABB<Dimensions, Type>& aabb);

        size_t computeSize(const std::unordered_map<Storage, Bounds>& elements);

        void forwardToLeaf(size_t size, const AABB<Dimensions, Type>& aabb);

        void populate(PermanentHeapPool& pool,
                      const std::unordered_map<Storage, Bounds>& elements);

        [[nodiscard]] bool isLeaf() const;

        [[nodiscard]] StaticTreeLeaf<Storage, Bounds, Dimensions, Type>&
        asLeaf();

        [[nodiscard]] const StaticTreeLeaf<Storage, Bounds, Dimensions, Type>&
        asLeaf() const;

        template<typename Collider>
        void intersections(
                const Collider& collider,
                std::unordered_set<TreeContent<Storage, Bounds>>& set,
                bool skipCollisionCheck) const;

        template<typename Collider>
        void forEachIntersection(
                const Collider& collider,
                std::function<void(
                        const TreeContent<Storage, Bounds>&)> consumer,
                bool skipCollisionCheck) const;
    };

    template<typename Storage, typename Bounds,
            size_t Dimensions, typename Type,
            size_t MaxObjects, size_t Depth> requires(Depth > 0)
    class StaticTreeNode {

        using ChildType = std::conditional_t<
                (Depth > 2),
                StaticTreeNode<
                        Storage, Bounds, Dimensions,
                        Type, MaxObjects, Depth - 1
                >,
                StaticTreeLeaf<Storage, Bounds, Dimensions, Type>
        >;

        AABB<Dimensions, Type> _aabb;
        ChildType _children[1 << Dimensions];
        bool _leaf;

    public:

        StaticTreeNode() = default;

        explicit StaticTreeNode(const AABB<Dimensions, Type>& aabb);

        size_t computeSize(const std::unordered_map<Storage, Bounds>& elements);

        void forwardToLeaf(size_t size, const AABB<Dimensions, Type>& aabb);

        void populate(PermanentHeapPool& pool,
                      const std::unordered_map<Storage, Bounds>& elements);

        [[nodiscard]] bool isLeaf() const;

        [[nodiscard]] StaticTreeLeaf<Storage, Bounds, Dimensions, Type>&
        asLeaf();

        [[nodiscard]] const StaticTreeLeaf<Storage, Bounds, Dimensions, Type>&
        asLeaf() const;

        template<typename Collider>
        void
        intersections(const Collider& collider,
                      std::unordered_set<TreeContent<Storage, Bounds>>& set,
                      bool skipCollisionCheck) const;

        template<typename Collider>
        void forEachIntersection(
                const Collider& collider,
                std::function<void(
                        const TreeContent<Storage, Bounds>&)> consumer,
                bool skipCollisionCheck) const;

    };

    template<typename Storage, typename Bounds,
            size_t Dimensions, typename Type,
            size_t MaxObjects, size_t Depth> requires(Depth > 0)
    class StaticTree {

        StaticTreeNode<Storage, Bounds, Dimensions, Type, MaxObjects, Depth> _root;
        PermanentHeapPool _pool;

    public:

        using Elements = std::unordered_map<Storage, Bounds>;

        using Content = rush::TreeContent<Storage, Bounds>;

        explicit StaticTree(const AABB<Dimensions, Type>& aabb,
                            const Elements& elements);

        const
        StaticTreeNode<Storage, Bounds, Dimensions, Type, MaxObjects, Depth>&
        getRoot() const;


        template<typename Collider>
        inline void intersections(
                const Collider& collider,
                std::unordered_set<TreeContent<Storage, Bounds>>& set) const;

        template<typename Collider>
        void forEachIntersection(
                const Collider& collider,
                std::function<void(
                        const TreeContent<Storage, Bounds>&)> consumer) const;

    };

}

#include <rush/geometry/static_tree_impl.h>

#endif //RUSH_STATIC_TREE_BASE_H
