//
// Created by gaelrialcostas on 3/06/24.
//

#ifndef RUSH_STATIC_TREE_IMPL_H
#define RUSH_STATIC_TREE_IMPL_H

namespace rush {

    template<typename Storage, typename Bounds, size_t Dimensions,
            typename Type>
    StaticTreeLeaf<Storage, Bounds, Dimensions, Type>::
    StaticTreeLeaf(const AABB<Dimensions, Type>& aabb) :
            _aabb(aabb), _elements(nullptr), _size(0) {
    }

    template<typename Storage, typename Bounds, size_t Dimensions,
            typename Type>
    size_t StaticTreeLeaf<Storage, Bounds, Dimensions, Type>::
    computeSize(const std::unordered_map<Storage, Bounds>& elements) {
        size_t count = 0;

        for (const auto& item: elements) {
            if (intersects(_aabb, item.second)) {
                ++count;
            }
        }

        _size = count;
        return count;
    }

    template<typename Storage, typename Bounds, size_t Dimensions,
            typename Type>
    void StaticTreeLeaf<Storage, Bounds, Dimensions, Type>::
    forwardToLeaf(size_t size, const AABB<Dimensions, Type>& aabb) {
        _size = size;
        _aabb = aabb;
    }

    template<typename Storage, typename Bounds, size_t Dimensions,
            typename Type>
    void StaticTreeLeaf<Storage, Bounds, Dimensions, Type>::
    populate(PermanentHeapPool& pool,
             const std::unordered_map<Storage, Bounds>& elements) {
        TreeContent<Storage, Bounds>* pivot = (TreeContent<Storage, Bounds>*)
                pool.allocate(_size * sizeof(TreeContent<Storage, Bounds>))
                        .value_or(nullptr);
        _elements = pivot;

        size_t count = 0;

        for (const auto& item: elements) {
            if (intersects(_aabb, item.second)) {
#ifndef NDEBUG
                if (count > _size) {
                    throw std::runtime_error("Incorrect population.");
                }
#endif
                pivot->storage = item.first;
                pivot->bounds = item.second;
                ++pivot;
                ++count;
#ifdef NDEBUG
                if (count == _size) break;
#endif
            }
        }
    }

    template<typename Storage, typename Bounds,
            size_t Dimensions, typename Type>
    bool StaticTreeLeaf<Storage, Bounds, Dimensions, Type>::
    isLeaf() const {
        return false;
    }

    template<typename Storage, typename Bounds,
            size_t Dimensions, typename Type>
    StaticTreeLeaf<Storage, Bounds, Dimensions, Type>&
    StaticTreeLeaf<Storage, Bounds, Dimensions, Type>::
    asLeaf() {
        return *this;
    }

    template<typename Storage, typename Bounds,
            size_t Dimensions, typename Type>
    const StaticTreeLeaf<Storage, Bounds, Dimensions, Type>&
    StaticTreeLeaf<Storage, Bounds, Dimensions, Type>::
    asLeaf() const {
        return *this;
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
    computeSize(const std::unordered_map<Storage, Bounds>& elements) {
        size_t count = 0;

        for (const auto& item: elements) {
            if (intersects(_aabb, item.second)) {
                ++count;
            }
        }

        if (count <= MaxObjects) {
            // This node is a leaf node.
            // We'll use only the first child
            // of this node to store the data.
            forwardToLeaf(count, _aabb);
            return count;
        }

        _leaf = false;
        count = 0;
        for (auto& child: _children) {
            count += child.computeSize(elements);
        }

        return count;
    }

    template<typename Storage, typename Bounds, size_t Dimensions,
            typename Type, size_t MaxObjects, size_t Depth>
    requires(Depth > 0)
    void StaticTreeNode<Storage, Bounds, Dimensions, Type, MaxObjects, Depth>::
    forwardToLeaf(size_t size, const AABB<Dimensions, Type>& aabb) {
        _children[0].forwardToLeaf(size, aabb);
    }

    template<typename Storage, typename Bounds, size_t Dimensions,
            typename Type, size_t MaxObjects, size_t Depth>
    requires(Depth > 0)
    void StaticTreeNode<Storage, Bounds, Dimensions, Type, MaxObjects, Depth>::
    populate(PermanentHeapPool& pool,
             const std::unordered_map<Storage, Bounds>& elements) {
        if (_leaf) {
            _children[0].populate(pool, elements);
        } else {
            for (auto& child: _children) {
                child.populate(pool, elements);
            }
        }
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
    StaticTreeLeaf<Storage, Bounds, Dimensions, Type>&
    StaticTreeNode<Storage, Bounds, Dimensions, Type, MaxObjects, Depth>::
    asLeaf() {
        return _children[0].asLeaf();
    }

    template<typename Storage, typename Bounds, size_t Dimensions,
            typename Type, size_t MaxObjects, size_t Depth>
    requires(Depth > 0)
    const StaticTreeLeaf<Storage, Bounds, Dimensions, Type>&
    StaticTreeNode<Storage, Bounds, Dimensions, Type, MaxObjects, Depth>::
    asLeaf() const {
        return _children[0].asLeaf();
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
        if (_leaf) {
            asLeaf().intersections(collider, set, true);
        } else {
            for (const auto& child: _children) {
                child.intersections(collider, set, skipCollisionCheck);
            }
        }
    }

    template<typename Storage, typename Bounds, size_t Dimensions,
            typename Type, size_t MaxObjects, size_t Depth>
    requires(Depth > 0)
    StaticTree<Storage, Bounds, Dimensions, Type, MaxObjects, Depth>::
    StaticTree(const AABB<Dimensions, Type>& aabb,
               const std::unordered_map<Storage, Bounds>& elements)
            : _root(aabb),
              _pool(_root.computeSize(elements) * sizeof(Content)) {
        _root.populate(_pool, elements);
    }

    template<typename Storage, typename Bounds, size_t Dimensions,
            typename Type, size_t MaxObjects, size_t Depth>
    requires(Depth > 0)
    const StaticTreeNode<Storage, Bounds, Dimensions, Type, MaxObjects, Depth>&
    StaticTree<Storage, Bounds, Dimensions, Type, MaxObjects, Depth>::
    getRoot() const {
        return _root;
    }

}

#endif //RUSH_STATIC_TREE_IMPL_H
