//
// Created by gaelr on 29/01/2024.
//

#ifndef TREE_IMPL_H
#define TREE_IMPL_H

#endif //TREE_IMPL_H


namespace rush {
    template<typename Storage, typename Bounds>
    bool TreeContent<Storage, Bounds>::operator==(const TreeContent& o) const {
        return storage == o.storage;
    }

    template<typename Storage, typename Bounds, size_t Dimensions, typename Type, size_t
    MaxObjects, size_t Depth, size_t MaxSubtrees, bool Root>
    void
    Tree<Storage, Bounds, Dimensions, Type, MaxObjects, Depth, MaxSubtrees,
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
    Tree<Storage, Bounds, Dimensions, Type, MaxObjects, Depth, MaxSubtrees,
            Root>::
    Tree() requires (Root == false)
            : _pool(nullptr),
              _aabb(),
              _children(nullptr),
              _contents(),
              _size(0),
              _leaf(true) {
    }

    template<typename Storage, typename Bounds, size_t Dimensions, typename Type,
            size_t MaxObjects, size_t Depth, size_t MaxSubtrees, bool Root>
    Tree<Storage, Bounds, Dimensions, Type, MaxObjects, Depth, MaxSubtrees,
            Root>::
    Tree(AABB<Dimensions, Type> aabb) requires (Root == true)
            : _pool(),
              _aabb(aabb),
              _children(nullptr),
              _contents(),
              _size(0),
              _leaf(true) {
    }

    template<typename Storage, typename Bounds, size_t Dimensions, typename Type, size_t MaxObjects, size_t Depth,
            size_t MaxSubtrees, bool Root>
    Tree<Storage, Bounds, Dimensions, Type, MaxObjects, Depth, MaxSubtrees,
            Root>::~Tree() {
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
    Tree<Storage, Bounds, Dimensions, Type, MaxObjects, Depth, MaxSubtrees,
            Root>::getStorage() {
        return _contents;
    }

    template<typename Storage, typename Bounds, size_t Dimensions, typename Type,
            size_t MaxObjects, size_t Depth, size_t MaxSubtrees, bool Root>
    const std::vector<TreeContent<Storage, Bounds>>&
    Tree<Storage, Bounds, Dimensions, Type, MaxObjects, Depth, MaxSubtrees,
            Root>::getStorage() const {
        return _contents;
    }

    template<typename Storage, typename Bounds, size_t Dimensions, typename Type,
            size_t MaxObjects, size_t Depth, size_t MaxSubtrees, bool Root>
    std::vector<AbstractTree<AABB<Dimensions, Type>, Storage, Bounds>*>
    Tree<Storage, Bounds, Dimensions, Type, MaxObjects, Depth, MaxSubtrees,
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
    Tree<Storage, Bounds, Dimensions, Type, MaxObjects, Depth, MaxSubtrees,
            Root>::
    getBounds() const {
        return _aabb;
    }


    template<typename Storage, typename Bounds, size_t Dimensions, typename Type,
            size_t MaxObjects, size_t Depth, size_t MaxSubtrees, bool Root>
    size_t
    Tree<Storage, Bounds, Dimensions, Type, MaxObjects, Depth, MaxSubtrees,
            Root>::size() const {
        return _size;
    }


    template<typename Storage, typename Bounds, size_t Dimensions, typename Type,
            size_t MaxObjects, size_t Depth, size_t MaxSubtrees, bool Root>
    TreeInsertResult
    Tree<Storage, Bounds, Dimensions, Type, MaxObjects, Depth, MaxSubtrees,
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
    Tree<Storage, Bounds, Dimensions, Type, MaxObjects, Depth, MaxSubtrees,
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
    Tree<Storage, Bounds, Dimensions, Type, MaxObjects, Depth, MaxSubtrees,
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
    Tree<Storage, Bounds, Dimensions, Type, MaxObjects, Depth, MaxSubtrees,
            Root>::isLeaf() const {
        return _leaf;
    }

    template<typename Storage, typename Bounds, size_t Dimensions, typename Type,
            size_t MaxObjects, size_t Depth, size_t MaxSubtrees, bool Root>
    TreeIterator<AABB<Dimensions, Type>, Storage, Bounds>
    Tree<Storage, Bounds, Dimensions, Type, MaxObjects, Depth, MaxSubtrees,
            Root>::
    begin() {
        return TreeIterator<AABB<Dimensions, Type>, Storage, Bounds>(this);
    }

    template<typename Storage, typename Bounds, size_t Dimensions, typename Type, size_t MaxObjects, size_t Depth, size_t MaxSubtrees, bool Root>
    template<typename FilterBounds>
    TreeIterator<AABB<Dimensions, Type>, Storage, Bounds, FilterBounds>
    Tree<Storage, Bounds, Dimensions, Type, MaxObjects, Depth, MaxSubtrees, Root>::
    begin(const FilterBounds& bounds) {
        return TreeIterator<AABB<Dimensions, Type>, Storage, Bounds, FilterBounds>(
                this, bounds
        );
    }

    template<typename Storage, typename Bounds, size_t Dimensions, typename Type, size_t MaxObjects, size_t Depth,
            size_t MaxSubtrees, bool Root>
    TreeIterator<AABB<Dimensions, Type>, Storage, Bounds>
    Tree<Storage, Bounds, Dimensions, Type, MaxObjects, Depth, MaxSubtrees,
            Root>::end() {
        return TreeIterator<AABB<Dimensions, Type>, Storage, Bounds>();
    }

    template<typename TreeBounds, typename Storage, typename Bounds,
            typename FilterBounds>
    TreeIterator<TreeBounds, Storage, Bounds, FilterBounds>::
    TreeIterator(AbstractTree<TreeBounds, Storage, Bounds>* root)
            : _current(root),
              _filter({}),
              _end(false) {
        if (_current->getStorage().empty()) {
            operator++();
        }
    }

    template<typename TreeBounds, typename Storage, typename Bounds,
            typename FilterBounds>
    TreeIterator<TreeBounds, Storage, Bounds, FilterBounds>::TreeIterator(
            AbstractTree<TreeBounds, Storage, Bounds>* root,
            const FilterBounds& filter)
            : _current(root),
              _filter(filter),
              _end(false) {
        if (_current->getStorage().empty()) {
            operator++();
        }
    }

    template<typename TreeBounds, typename Storage, typename Bounds,
            typename FilterBounds>
    TreeIterator<TreeBounds, Storage, Bounds, FilterBounds>::TreeIterator()
            : _current(nullptr),
              _filter({}),
              _end(true) {
    }

    template<typename TreeBounds, typename Storage, typename Bounds,
            typename FilterBounds>
    AbstractTree<TreeBounds, Storage, Bounds>*
    TreeIterator<TreeBounds, Storage, Bounds, FilterBounds>::getTree() const {
        return _current;
    }


    template<typename TreeBounds, typename Storage, typename Bounds,
            typename FilterBounds>
    const typename TreeIterator<TreeBounds, Storage, Bounds, FilterBounds>::reference
    TreeIterator<TreeBounds, Storage, Bounds, FilterBounds>::operator*() const {
        return _current->getStorage();
    }

    template<typename TreeBounds, typename Storage, typename Bounds,
            typename FilterBounds>
    typename TreeIterator<TreeBounds, Storage, Bounds, FilterBounds>::pointer
    TreeIterator<TreeBounds, Storage, Bounds, FilterBounds>::operator->() const {
        return &_current->getStorage();
    }

    template<typename TreeBounds, typename Storage, typename Bounds,
            typename FilterBounds>
    TreeIterator<TreeBounds, Storage, Bounds, FilterBounds>&
    TreeIterator<TreeBounds, Storage, Bounds, FilterBounds>::operator++() {
        const FilterBounds* filter = _filter.has_value() ? &_filter.value()
                                                         : nullptr;
        if (_end) return *this;
        do {
            // Add children to queue.
            for (auto child: _current->getChildren()) {
                if (filter == nullptr || rush::intersects(
                        *filter, child->getBounds())) {
                    _queue.push_back(child);
                }
            }

            if (!_queue.empty()) {
                _current = _queue.back();
                _queue.pop_back();
            } else {
                _current = nullptr;
                _end = true;
            }
        } while (!_end && _current->getStorage().empty());

        return *this;
    }

    template<typename TreeBounds, typename Storage, typename Bounds,
            typename FilterBounds>
    TreeIterator<TreeBounds, Storage, Bounds, FilterBounds>
    TreeIterator<TreeBounds, Storage, Bounds, FilterBounds>::operator++(int) {
        auto tmp = *this;
        ++(*this);
        return tmp;
    }

    template<typename TreeBounds, typename Storage, typename Bounds,
            typename FilterBounds>
    bool TreeIterator<TreeBounds, Storage, Bounds, FilterBounds>::operator==(
            const TreeIterator& p) {
        return (_end && p._end) || _current == p._current;
    }

    template<typename TreeBounds, typename Storage, typename Bounds,
            typename FilterBounds>
    bool TreeIterator<TreeBounds, Storage, Bounds, FilterBounds>::operator!=(
            const TreeIterator& p) {
        return !((_end && p._end) || _current == p._current);
    }
}
