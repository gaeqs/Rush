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

        size_t populate(TreeContent<Storage, Bounds>* pool,
                        size_t from, size_t to);

        [[nodiscard]] const AABB<Dimensions, Type>& getBounds() const;

        [[nodiscard]] bool isLeaf() const;

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
        TreeContent<Storage, Bounds>* _elements;
        size_t _size;

        ChildType _children[1 << Dimensions];
        bool _leaf;

    public:
        StaticTreeNode() = default;

        explicit StaticTreeNode(const AABB<Dimensions, Type>& aabb);

        size_t populate(TreeContent<Storage, Bounds>* pool,
                        size_t from, size_t to);

        [[nodiscard]] const AABB<Dimensions, Type>& getBounds() const;

        [[nodiscard]] bool isLeaf() const;

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
        StaticTreeNode<Storage, Bounds, Dimensions, Type, MaxObjects, Depth>
        _root;
        TreeContent<Storage, Bounds>* _pool;
        size_t _size;

    public:
        using Content = rush::TreeContent<Storage, Bounds>;
        using Elements = std::vector<Content>;


        explicit StaticTree(const AABB<Dimensions, Type>& aabb,
                            const Elements& elements);

        ~StaticTree();

        const
        StaticTreeNode<Storage, Bounds, Dimensions, Type, MaxObjects, Depth>&
        getRoot() const;

        template<typename Collider>
        void intersections(
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
