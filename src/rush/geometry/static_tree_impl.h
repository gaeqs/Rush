//
// Created by gaelrialcostas on 3/06/24.
//

#ifndef RUSH_STATIC_TREE_IMPL_H
#define RUSH_STATIC_TREE_IMPL_H

namespace rush {
    template<typename Storage, typename Bounds, size_t Dimensions,
        typename Type>
    StaticTreeLeaf<Storage, Bounds, Dimensions, Type>::
    StaticTreeLeaf(const AABB<Dimensions, Type>& aabb) : _aabb(aabb),
        _elements(nullptr), _size(0) {
    }

    template<typename Storage, typename Bounds, size_t Dimensions,
        typename Type>
    size_t StaticTreeLeaf<Storage, Bounds, Dimensions, Type>::
    populate(TreeContent<Storage, Bounds>* pool,
             size_t from, size_t to) {
        _elements = pool + from;

        // Recognition pass
        size_t pivot = from;
        for (size_t i = from; i < to; ++i) {
            if (!intersects(pool[i].bounds, _aabb)) continue;
            if (pivot != i) {
                std::swap(pool[i], pool[pivot]);
            }
            ++pivot;
        }

        _size = pivot - from;
        return pivot;
    }

    template<typename Storage, typename Bounds,
        size_t Dimensions, typename Type>
    const AABB<Dimensions, Type>&
    StaticTreeLeaf<Storage, Bounds, Dimensions, Type>::
    getBounds() const {
        return _aabb;
    }

    template<typename Storage, typename Bounds,
        size_t Dimensions, typename Type>
    bool StaticTreeLeaf<Storage, Bounds, Dimensions, Type>::
    isLeaf() const {
        return true;
    }

    template<typename Storage, typename Bounds,
        size_t Dimensions, typename Type>
    template<typename Collider>
    void
    StaticTreeLeaf<Storage, Bounds, Dimensions, Type>::
    intersections(
        const Collider& collider,
        std::unordered_set<TreeContent<Storage, Bounds>>& set,
        bool skipCollisionCheck) const {
        if (!skipCollisionCheck && !intersects(_aabb, collider)) return;

        for (size_t i = 0; i < _size; ++i) {
            TreeContent<Storage, Bounds>* current = _elements + i;
            if (intersects(current->bounds, collider)) {
                set.insert(*current);
            }
        }
    }

    template<typename Storage, typename Bounds,
        size_t Dimensions, typename Type>
    template<typename Collider>
    void StaticTreeLeaf<Storage, Bounds, Dimensions, Type>::
    forEachIntersection(
        const Collider& collider,
        std::function<void(const TreeContent<Storage, Bounds>&)> consumer,
        bool skipCollisionCheck) const {
        if (!skipCollisionCheck && !intersects(_aabb, collider)) return;

        for (size_t i = 0; i < _size; ++i) {
            TreeContent<Storage, Bounds>* current = _elements + i;
            if (intersects(current->bounds, collider)) {
                consumer(*current);
            }
        }
    }

    template<typename Storage, typename Bounds, size_t Dimensions,
        typename Type, size_t MaxObjects, size_t Depth>
        requires(Depth > 0)
    StaticTreeNode<Storage, Bounds, Dimensions, Type, MaxObjects, Depth>::
    StaticTreeNode(const AABB<Dimensions, Type>& aabb)
        : _aabb(aabb), _leaf(true) {
        auto childrenRadius = _aabb.radius / 2;
        for (size_t i = 0; i < (1 << Dimensions); ++i) {
            auto localCenter = _aabb.center;
            for (size_t d = 0; d < Dimensions; ++d) {
                if ((i >> d & 1) == 1) {
                    localCenter[d] -= childrenRadius[d];
                } else {
                    localCenter[d] += childrenRadius[d];
                }
            }

            _children[i] = ChildType(AABB(localCenter, childrenRadius));
        }
    }

    template<typename Storage, typename Bounds, size_t Dimensions,
        typename Type, size_t MaxObjects, size_t Depth>
        requires(Depth > 0)
    size_t
    StaticTreeNode<Storage, Bounds, Dimensions, Type, MaxObjects, Depth>::
    populate(TreeContent<Storage, Bounds>* pool,
             size_t from, size_t to) {
        _elements = pool + from;

        // Recognition pass
        size_t pivot = from;
        for (size_t i = from; i < to; ++i) {
            if (!intersects(pool[i].bounds, _aabb)) continue;
            if (pivot != i) {
                std::swap(pool[i], pool[pivot]);
            }
            ++pivot;
        }

        _size = pivot - from;

        if (_size <= MaxObjects) return pivot;

        // Let's fetch only the elements that are in more than
        // one child.

        to = pivot;
        pivot = from;

        // Let's store only the elements
        // that are inside more than one child.
        // (Or not inside any children)
        for (size_t i = from; i < to; ++i) {
            size_t childCount = 0;
            for (const auto& child: _children) {
                if (intersects(pool[i].bounds, child.getBounds())) {
                    ++childCount;
                    if (childCount > 1) break;
                }
            }

            // Special case: maybe the element is not inside any
            // children but inside the parent?
            if (childCount != 1) continue;
            if (pivot != i) {
                std::swap(pool[i], pool[pivot]);
            }
            ++pivot;
        }

        _size = pivot - from;

        if (pivot < to) {
            _leaf = false;
            for (auto& child: _children) {
                pivot = child.populate(pool, pivot, to);
            }
        }

        return to;
    }

    template<typename Storage, typename Bounds, size_t Dimensions,
        typename Type, size_t MaxObjects, size_t Depth>
        requires(Depth > 0)
    const AABB<Dimensions, Type>&
    StaticTreeNode<Storage, Bounds, Dimensions, Type, MaxObjects, Depth>::
    getBounds() const {
        return _aabb;
    }

    template<typename Storage, typename Bounds, size_t Dimensions,
        typename Type, size_t MaxObjects, size_t Depth>
        requires (Depth > 0)
    const std::array<typename StaticTreeNode<Storage, Bounds,
        Dimensions, Type, MaxObjects, Depth>::ChildType, 1 << Dimensions>&
    StaticTreeNode<Storage, Bounds, Dimensions, Type, MaxObjects, Depth>::
    getChildren() const {
        return _children;
    }


    template<typename Storage, typename Bounds, size_t Dimensions,
        typename Type, size_t MaxObjects, size_t Depth>
        requires(Depth > 0)
    bool StaticTreeNode<Storage, Bounds, Dimensions, Type, MaxObjects, Depth>::
    isLeaf() const {
        return _leaf;
    }

    template<typename Storage, typename Bounds, size_t Dimensions,
        typename Type, size_t MaxObjects, size_t Depth>
        requires(Depth > 0)
    template<typename Collider>
    void
    StaticTreeNode<Storage, Bounds, Dimensions, Type, MaxObjects, Depth>::
    intersections(
        const Collider& collider,
        std::unordered_set<TreeContent<Storage, Bounds>>& set,
        bool skipCollisionCheck) const {
        if (!skipCollisionCheck && !intersects(_aabb, collider)) return;

        for (size_t i = 0; i < _size; ++i) {
            TreeContent<Storage, Bounds>* current = _elements + i;
            if (intersects(current->bounds, collider)) {
                set.insert(*current);
            }
        }

        if (_leaf) return;
        for (const auto& child: _children) {
            child.intersections(collider, set, skipCollisionCheck);
        }
    }

    template<typename Storage, typename Bounds, size_t Dimensions, typename Type
        , size_t MaxObjects, size_t Depth>
        requires(Depth > 0)
    template<typename Collider>
    void
    StaticTreeNode<Storage, Bounds, Dimensions, Type, MaxObjects, Depth>::
    forEachIntersection(
        const Collider& collider,
        std::function<void(const TreeContent<Storage, Bounds>&)> consumer,
        bool skipCollisionCheck) const {
        if (!skipCollisionCheck && !intersects(_aabb, collider)) return;

        for (size_t i = 0; i < _size; ++i) {
            TreeContent<Storage, Bounds>* current = _elements + i;
            if (intersects(current->bounds, collider)) {
                consumer(*current);
            }
        }

        if (_leaf) return;
        for (const auto& child: _children) {
            child.forEachIntersection(collider, consumer, skipCollisionCheck);
        }
    }

    template<typename Storage, typename Bounds, size_t Dimensions, typename Type
        , size_t MaxObjects, size_t Depth>
        requires (Depth > 0)
    template<typename RAllocator>
    RayCastResult<Dimensions, Type>
    StaticTreeNode<Storage, Bounds, Dimensions, Type, MaxObjects, Depth>::
    raycast(Ray<Dimensions, Type, RAllocator> ray) const {
        RayCastResult<Dimensions, Type> boxHit;
        raycast(ray, _aabb, boxHit);
        if (!boxHit.hit) return boxHit;

        RayCastResult<Dimensions, Type> childrenResult;

        // Let's check the childrens for any hit.
        if (!_leaf) {
            RayCastResult<Dimensions, Type> results[1 << Dimensions];
            size_t amount = 0;

            for (size_t i = 0; i < 1 << Dimensions; ++i) {
                auto bounds = _children[i].getBounds();
                raycast(ray, bounds, results[amount]);
                if (results[amount].hit) ++amount;
            }

            // Find the minimum hit
            while (amount > 0) {
                size_t selected = 0;
                Type distance = std::numeric_limits<Type>::max();
                for (size_t i = 0; i < amount; ++i) {
                    if (distance > results[i].distance) {
                        distance = results[i].distance;
                        selected = i;
                    }
                }

                childrenResult = _children[selected].raycast(ray);
                if (childrenResult.hit) break;

                --amount;
                childrenResult[selected] = childrenResult[amount];
            }
        }

        // Let's check the contents now
        RayCastResult<Dimensions, Type> contentsResult;
        for (size_t i = 0; i < _size; ++i) {
            RayCastResult<Dimensions, Type> result;
            raycast(ray, _elements[i].bounds);
            if (result.hit) {
                if (!contentsResult.hit ||
                    contentsResult.distance > result.distance) {
                    contentsResult = result;
                }
            }
        }

        // Let's finally compare
        if (!childrenResult.hit) return contentsResult;
        if (!contentsResult.hit) return childrenResult;
        return childrenResult.distance > contentsResult.distance
                   ? contentsResult
                   : childrenResult;
    }


    template<typename Storage, typename Bounds, size_t Dimensions,
        typename Type, size_t MaxObjects, size_t Depth>
        requires(Depth > 0)
    StaticTree<Storage, Bounds, Dimensions, Type, MaxObjects, Depth>::
    StaticTree(const AABB<Dimensions, Type>& aabb,
               const Elements& elements)
        : _root(aabb),
          _pool(new TreeContent<Storage, Bounds>[elements.size()]),
          _size(elements.size()) {
        // Let's fill the pool.
        std::copy(elements.cbegin(), elements.cend(), _pool);
        _root.populate(_pool, 0, _size);
    }

    template<typename Storage, typename Bounds, size_t Dimensions,
        typename Type, size_t MaxObjects, size_t Depth>
        requires(Depth > 0)
    StaticTree<Storage, Bounds, Dimensions, Type, MaxObjects, Depth>::
    ~StaticTree() {
        delete[] _pool;
    }


    template<typename Storage, typename Bounds, size_t Dimensions,
        typename Type, size_t MaxObjects, size_t Depth>
        requires(Depth > 0)
    const StaticTreeNode<Storage, Bounds, Dimensions, Type, MaxObjects, Depth>&
    StaticTree<Storage, Bounds, Dimensions, Type, MaxObjects, Depth>::
    getRoot() const {
        return _root;
    }

    template<typename Storage, typename Bounds, size_t Dimensions,
        typename Type, size_t MaxObjects, size_t Depth>
        requires(Depth > 0)
    template<typename Collider>
    void
    StaticTree<Storage, Bounds, Dimensions, Type, MaxObjects, Depth>::
    intersections(const Collider& collider,
                  std::unordered_set<TreeContent<Storage, Bounds>>& set) const {
        _root.intersections(collider, set, false);
    }


    template<typename Storage, typename Bounds, size_t Dimensions,
        typename Type, size_t MaxObjects, size_t Depth>
        requires(Depth > 0)
    template<typename Collider>
    void
    StaticTree<Storage, Bounds, Dimensions, Type, MaxObjects, Depth>::
    forEachIntersection(
        const Collider& collider, std::function<void(
            const TreeContent<Storage, Bounds>&)> consumer) const {
        _root.forEachIntersection(collider, consumer, false);
    }
}

#endif //RUSH_STATIC_TREE_IMPL_H
