//
// Created by gaelrialcostas on 3/06/24.
//

#ifndef RUSH_DYNAMIC_TREE_IMPL_H
#define RUSH_DYNAMIC_TREE_IMPL_H

#endif //RUSH_DYNAMIC_TREE_IMPL_H

namespace rush {
    template<typename Storage, typename Bounds, size_t Dimensions, typename Type, size_t
    MaxObjects, size_t Depth, size_t MaxSubtrees, bool Root>
    void
    DynamicTree<Storage, Bounds, Dimensions, Type, MaxObjects, Depth, MaxSubtrees,
            Root>::split() {
        if constexpr (Depth > 0) {
            std::optional<void*> optional;

            if constexpr (Root) {
                optional = _pool.allocate();
            } else {
                optional = _pool->allocate();
            }

            if (!optional.has_value()) return;

            _children = static_cast<Children*>(optional.value());
            std::construct_at<Children>(_children);

            auto childrenRadius = _aabb.radius / 2;
            for (size_t i = 0; i < _children->size(); ++i) {
                auto localCenter = _aabb.center;
                for (size_t d = 0; d < Dimensions; ++d) {
                    if ((i >> d & 1) == 1) {
                        localCenter[d] -= childrenRadius[d];
                    } else {
                        localCenter[d] += childrenRadius[d];
                    }
                }

                auto& child = (*_children)[i];
                child._aabb = rush::AABB<Dimensions, Type>(localCenter,
                                                           childrenRadius);

                if constexpr (Depth > 1) {
                    if constexpr (Root) {
                        child._pool = &_pool;
                    } else {
                        child._pool = _pool;
                    }
                }
            }

            for (auto& child: *_children) {
                for (auto& content: _contents) {
                    child.insert(content.storage, content.bounds);
                }
            }

            _contents.clear();
            _leaf = false;
        }
    }

    template<typename Storage, typename Bounds, size_t Dimensions, typename Type, size_t MaxObjects, size_t Depth,
            size_t MaxSubtrees, bool Root>
    DynamicTree<Storage, Bounds, Dimensions, Type, MaxObjects, Depth, MaxSubtrees,
            Root>::
    DynamicTree() requires (!Root)
            : _pool(nullptr),
              _aabb(),
              _children(nullptr),
              _contents(),
              _size(0),
              _leaf(true) {
    }

    template<typename Storage, typename Bounds, size_t Dimensions, typename Type,
            size_t MaxObjects, size_t Depth, size_t MaxSubtrees, bool Root>
    DynamicTree<Storage, Bounds, Dimensions, Type, MaxObjects, Depth, MaxSubtrees,
            Root>::
    DynamicTree(AABB<Dimensions, Type> aabb) requires (Root)
            : _pool(),
              _aabb(aabb),
              _children(nullptr),
              _contents(),
              _size(0),
              _leaf(true) {
    }

    template<typename Storage, typename Bounds, size_t Dimensions, typename Type, size_t MaxObjects, size_t Depth,
            size_t MaxSubtrees, bool Root>
    DynamicTree<Storage, Bounds, Dimensions, Type, MaxObjects, Depth, MaxSubtrees,
            Root>::~DynamicTree() {
        if (_children != nullptr) {
            std::destroy_at<Children>(_children);
            if constexpr (Root) {
                _pool.deallocate(_children);
            } else {
                _pool->deallocate(_children);
            }
            _children = nullptr;
        }
    }

    template<typename Storage, typename Bounds, size_t Dimensions, typename Type,
            size_t MaxObjects, size_t Depth, size_t MaxSubtrees, bool Root>
    std::vector<TreeContent<Storage, Bounds>>&
    DynamicTree<Storage, Bounds, Dimensions, Type, MaxObjects, Depth, MaxSubtrees,
            Root>::getStorage() {
        return _contents;
    }

    template<typename Storage, typename Bounds, size_t Dimensions, typename Type,
            size_t MaxObjects, size_t Depth, size_t MaxSubtrees, bool Root>
    const std::vector<TreeContent<Storage, Bounds>>&
    DynamicTree<Storage, Bounds, Dimensions, Type, MaxObjects, Depth, MaxSubtrees,
            Root>::getStorage() const {
        return _contents;
    }

    template<typename Storage, typename Bounds, size_t Dimensions, typename Type,
            size_t MaxObjects, size_t Depth, size_t MaxSubtrees, bool Root>
    std::vector<AbstractTree<AABB<Dimensions, Type>, Storage, Bounds>*>
    DynamicTree<Storage, Bounds, Dimensions, Type, MaxObjects, Depth, MaxSubtrees,
            Root>::getChildren() const {
        if constexpr (Depth > 0) {
            if (_leaf) return {};

            std::vector<AbstractTree<AABB<Dimensions, Type>, Storage, Bounds>*> v;
            for (auto& child: *_children) {
                v.push_back(&child);
            }
            return v;
        }
        return {};
    }

    template<typename Storage, typename Bounds, size_t Dimensions, typename Type,
            size_t MaxObjects, size_t Depth, size_t MaxSubtrees, bool Root>
    const AABB<Dimensions, Type>&
    DynamicTree<Storage, Bounds, Dimensions, Type, MaxObjects, Depth, MaxSubtrees,
            Root>::
    getBounds() const {
        return _aabb;
    }


    template<typename Storage, typename Bounds, size_t Dimensions, typename Type,
            size_t MaxObjects, size_t Depth, size_t MaxSubtrees, bool Root>
    size_t
    DynamicTree<Storage, Bounds, Dimensions, Type, MaxObjects, Depth, MaxSubtrees,
            Root>::size() const {
        return _size;
    }


    template<typename Storage, typename Bounds, size_t Dimensions, typename Type,
            size_t MaxObjects, size_t Depth, size_t MaxSubtrees, bool Root>
    TreeInsertResult
    DynamicTree<Storage, Bounds, Dimensions, Type, MaxObjects, Depth, MaxSubtrees,
            Root>::
    insert(const Storage& storage, const Bounds& bounds) {
        if (!rush::intersects(_aabb, bounds)) {
            return remove(storage)
                   ? TreeInsertResult::REMOVED
                   : TreeInsertResult::NOTHING;
        }

        if (_leaf) {
            bool removed = remove(storage);
            _contents.push_back({bounds, storage});
            if constexpr (Depth > 0) {
                if (_contents.size() > MaxObjects) {
                    split();
                }
            }
            _size = _contents.size();
            return removed
                   ? TreeInsertResult::UPDATED
                   : TreeInsertResult::ADDED;
        }

        bool anyAdded = false;
        bool anyRemoved = false;
        bool anyUpdated = false;

        if constexpr (Depth > 0) {
            for (auto& child: *_children) {
                switch (child.insert(storage, bounds)) {
                    case TreeInsertResult::ADDED:
                        anyAdded = true;
                        break;
                    case TreeInsertResult::REMOVED:
                        anyRemoved = true;
                        break;
                    case TreeInsertResult::UPDATED:
                        anyUpdated = true;
                        break;
                    default:
                        break;
                }
            }
        }

        if (anyUpdated || anyAdded && anyRemoved)
            return TreeInsertResult::UPDATED;
        if (anyAdded) {
            ++_size;
            return TreeInsertResult::ADDED;
        }
        if (anyRemoved) {
            --_size;
            return TreeInsertResult::REMOVED;
        }
        return TreeInsertResult::NOTHING;
    }

    template<typename Storage, typename Bounds, size_t Dimensions, typename Type,
            size_t MaxObjects, size_t Depth, size_t MaxSubtrees, bool Root>
    bool
    DynamicTree<Storage, Bounds, Dimensions, Type, MaxObjects, Depth, MaxSubtrees,
            Root>::remove(
            const Storage& storage) {
        if (_leaf) {
            bool removed = std::erase_if(
                    _contents,
                    [storage
                    ](const TreeContent<Storage, Bounds>& it) {
                        return it.storage == storage;
                    }
            ) > 0;
            _size = _contents.size();
            return removed;
        }

        if constexpr (Depth > 0) {
            bool removed = false;
            for (auto& child: *_children) {
                removed |= child.remove(storage);
            }

            if (removed) --_size;

            if (size() <= MaxObjects) {
                auto localContains = [this](auto& s) {
                    return std::any_of(
                            _contents.begin(),
                            _contents.end(),
                            [&s](TreeContent<Storage, Bounds>& item) {
                                return item.storage == s;
                            }
                    );
                };


                for (auto& children: *this) {
                    for (auto& item: children) {
                        if (localContains(item.storage)) continue;
                        _contents.push_back(item);
                    }
                }

                std::destroy_at(_children);
                if constexpr (Root) {
                    _pool.deallocate(_children);
                } else {
                    _pool->deallocate(_children);
                }
                _children = nullptr;

                _leaf = true;
            }

            return removed;
        }
        return false;
    }

    template<typename Storage, typename Bounds, size_t Dimensions, typename Type,
            size_t MaxObjects, size_t Depth, size_t MaxSubtrees, bool Root>
    bool
    DynamicTree<Storage, Bounds, Dimensions, Type, MaxObjects, Depth, MaxSubtrees,
            Root>::contains(
            const Storage& storage) {
        if (_leaf) {
            return std::any_of(
                    _contents.begin(),
                    _contents.end(),
                    [&storage](TreeContent<Storage, Bounds>& item) {
                        return item.storage == storage;
                    }
            );
        }
        if constexpr (Depth > 0) {
            return std::any_of(
                    _children->begin(),
                    _children->end(),
                    [&storage](ChildType& child) {
                        return child.contains(storage);
                    }
            );
        }

        return false;
    }

    template<typename Storage, typename Bounds, size_t Dimensions, typename Type,
            size_t MaxObjects, size_t Depth, size_t MaxSubtrees, bool Root>
    bool
    DynamicTree<Storage, Bounds, Dimensions, Type, MaxObjects, Depth, MaxSubtrees,
            Root>::isLeaf() const {
        return _leaf;
    }

    template<typename Storage, typename Bounds, size_t Dimensions, typename Type,
            size_t MaxObjects, size_t Depth, size_t MaxSubtrees, bool Root>
    TreeIterator<AABB<Dimensions, Type>, Storage, Bounds>
    DynamicTree<Storage, Bounds, Dimensions, Type, MaxObjects, Depth, MaxSubtrees,
            Root>::
    begin() {
        return TreeIterator<AABB<Dimensions, Type>, Storage, Bounds>(this);
    }

    template<typename Storage, typename Bounds, size_t Dimensions, typename Type, size_t MaxObjects, size_t Depth, size_t MaxSubtrees, bool Root>
    template<typename FilterBounds>
    TreeIterator<AABB<Dimensions, Type>, Storage, Bounds, FilterBounds>
    DynamicTree<Storage, Bounds, Dimensions, Type, MaxObjects, Depth, MaxSubtrees, Root>::
    begin(const FilterBounds& bounds) {
        return TreeIterator<AABB<Dimensions, Type>, Storage, Bounds, FilterBounds>(
                this, bounds
        );
    }

    template<typename Storage, typename Bounds, size_t Dimensions, typename Type, size_t MaxObjects, size_t Depth,
            size_t MaxSubtrees, bool Root>
    TreeIterator<AABB<Dimensions, Type>, Storage, Bounds>
    DynamicTree<Storage, Bounds, Dimensions, Type, MaxObjects, Depth, MaxSubtrees,
            Root>::end() {
        return TreeIterator<AABB<Dimensions, Type>, Storage, Bounds>();
    }
}