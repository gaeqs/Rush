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
        MaxObjects, size_t Depth>
    void Tree<Storage, Bounds,Dimensions, Type, MaxObjects, Depth>::split() {
        if constexpr (Depth > 0) {
            auto childrenRadius = _aabb.radius / 2;

            _children = std::make_unique<std::array<ChildType,
                1 << Dimensions>>();

            for (size_t i = 0; i < _children->size(); ++i) {
                auto localCenter = _aabb.center;
                for (size_t d = 0; d < Dimensions; ++d) {
                    if ((i >> d & 1) == 1) {
                        localCenter[d] -= childrenRadius[d];
                    }
                    else {
                        localCenter[d] += childrenRadius[d];
                    }
                }

                (*_children)[i]._aabb = rush::AABB<Dimensions, Type>
                        (localCenter, childrenRadius);
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

    template<typename Storage, typename Bounds, size_t Dimensions, typename Type,
        size_t MaxObjects, size_t Depth>
    Tree<Storage, Bounds,Dimensions, Type, MaxObjects, Depth>::Tree()
        : _aabb(), _size(0), _leaf(true) {
    }

    template<typename Storage, typename Bounds, size_t Dimensions, typename Type,
        size_t MaxObjects, size_t Depth>
    Tree<Storage, Bounds,Dimensions, Type, MaxObjects, Depth>::
    Tree(AABB<Dimensions, Type> aabb)
        : _aabb(aabb), _size(0), _leaf(true) {
    }

    template<typename Storage, typename Bounds, size_t Dimensions, typename Type, size_t
        MaxObjects, size_t Depth>
    std::vector<TreeContent<Storage, Bounds>>&
    Tree<Storage, Bounds,Dimensions, Type, MaxObjects, Depth>::getStorage() {
        return _contents;
    }

    template<typename Storage, typename Bounds, size_t Dimensions, typename Type, size_t
        MaxObjects, size_t Depth>
    const std::vector<TreeContent<Storage, Bounds>>&
    Tree<Storage, Bounds,Dimensions, Type, MaxObjects, Depth>::getStorage() const {
        return _contents;
    }

    template<typename Storage, typename Bounds, size_t Dimensions, typename Type, size_t
        MaxObjects, size_t Depth>
    std::vector<AbstractTree<Storage, Bounds>*>
    Tree<Storage, Bounds,Dimensions, Type, MaxObjects, Depth>::getChildren() const {
        if constexpr (Depth > 0) {
            if (_leaf) return {};

            std::vector<AbstractTree<Storage, Bounds>*> v;
            for (auto& child: *_children) {
                v.push_back(&child);
            }
            return v;
        }
        return {};
    }

    template<typename Storage, typename Bounds, size_t Dimensions, typename Type, size_t
        MaxObjects, size_t Depth>
    std::any Tree<Storage, Bounds,Dimensions, Type, MaxObjects, Depth>::
    getBounds() const {
        return _aabb;
    }


    template<typename Storage, typename Bounds, size_t Dimensions, typename Type,
        size_t MaxObjects, size_t Depth>
    size_t Tree<Storage, Bounds,Dimensions, Type, MaxObjects, Depth>::size() const {
        return _size;
    }


    template<typename Storage, typename Bounds, size_t Dimensions, typename Type, size_t
        MaxObjects, size_t Depth>
    TreeInsertResult Tree<Storage, Bounds, Dimensions, Type, MaxObjects, Depth>::
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

    template<typename Storage, typename Bounds, size_t Dimensions, typename Type, size_t
        MaxObjects, size_t Depth>
    bool Tree<Storage, Bounds,Dimensions, Type, MaxObjects, Depth>::remove(
        const Storage& storage) {
        if (_leaf) {
            bool removed = std::erase_if(
                               _contents,
                               [storage](const TreeContent<Storage, Bounds>& it) {
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

                _leaf = true;
                _children = nullptr;
            }

            return removed;
        }
        return false;
    }

    template<typename Storage, typename Bounds, size_t Dimensions, typename Type, size_t
        MaxObjects, size_t Depth>
    bool Tree<Storage, Bounds,Dimensions, Type, MaxObjects, Depth>::contains(
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
        size_t MaxObjects, size_t Depth>
    bool Tree<Storage, Bounds,Dimensions, Type, MaxObjects, Depth>::isLeaf() const {
        return _leaf;
    }

    template<typename Storage, typename Bounds, size_t Dimensions, typename Type, size_t
        MaxObjects, size_t Depth>
    TreeIterator<Storage, Bounds> Tree<Storage, Bounds,Dimensions, Type, MaxObjects, Depth>::
    begin() {
        return TreeIterator<Storage, Bounds>(this);
    }

    template<typename Storage, typename Bounds, size_t Dimensions, typename Type, size_t
        MaxObjects, size_t Depth>
    TreeIterator<Storage, Bounds>
    Tree<Storage, Bounds,Dimensions, Type, MaxObjects, Depth>::
    end() {
        return TreeIterator<Storage, Bounds>();
    }

    template<typename Storage, typename Bounds>
    TreeIterator<Storage, Bounds>::TreeIterator(AbstractTree<Storage, Bounds>* root)
        : _root(root),
          _current(root),
          _end(false) {
        if (_current->getStorage().empty()) {
            operator++();
        }
    }

    template<typename Storage, typename Bounds>
    TreeIterator<Storage, Bounds>::TreeIterator()
        : _root(nullptr),
          _current(nullptr),
          _end(true) {
    }

    template<typename Storage, typename Bounds>
    AbstractTree<Storage, Bounds>*
    TreeIterator<Storage, Bounds>::getTree() const {
        return _current;
    }


    template<typename Storage, typename Bounds>
    const typename TreeIterator<Storage, Bounds>::reference
    TreeIterator<Storage, Bounds>::operator*() const {
        return _current->getStorage();
    }

    template<typename Storage, typename Bounds>
    typename TreeIterator<Storage, Bounds>::pointer
    TreeIterator<Storage, Bounds>::operator->() const {
        return &_current->getStorage();
    }

    template<typename Storage, typename Bounds>
    TreeIterator<Storage, Bounds>&
    TreeIterator<Storage, Bounds>::operator++() {
        if (_end) return *this;
        do {
            // Add children to queue.
            for (auto child: _current->getChildren()) {
                _queue.push_back(child);
            }

            if (!_queue.empty()) {
                _current = _queue.back();
                _queue.pop_back();
            }
            else {
                _current = nullptr;
                _end = true;
            }
        }
        while (!_end && _current->getStorage().empty());

        return *this;
    }

    template<typename Storage, typename Bounds>
    TreeIterator<Storage, Bounds>
    TreeIterator<Storage, Bounds>::operator++(Storage) {
        auto tmp = *this;
        ++(*this);
        return tmp;
    }

    template<typename Storage, typename Bounds>
    bool TreeIterator<Storage, Bounds>::operator==(const TreeIterator& p) {
        return (_end && p._end) || _current == p._current;
    }

    template<typename Storage, typename Bounds>
    bool TreeIterator<Storage, Bounds>::operator!=(const TreeIterator& p) {
        return !((_end && p._end) || _current == p._current);
    }
}
