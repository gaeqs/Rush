//
// Created by gaelr on 29/01/2024.
//

#ifndef TREE_IMPL_H
#define TREE_IMPL_H

#endif //TREE_IMPL_H


namespace rush {
    template<typename Storage>
    bool TreeContent<Storage>::operator==(const TreeContent& o) const {
        return storage == o.storage;
    }

    template<typename Storage, size_t Dimensions, typename Type, size_t
        MaxObjects, size_t Depth>
    void Tree<Storage, Dimensions, Type, MaxObjects, Depth>::split() {
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

    template<typename Storage, size_t Dimensions, typename Type,
        size_t MaxObjects, size_t Depth>
    Tree<Storage, Dimensions, Type, MaxObjects, Depth>::Tree()
        : _aabb(), _size(0), _leaf(true) {
    }

    template<typename Storage, size_t Dimensions, typename Type,
        size_t MaxObjects, size_t Depth>
    Tree<Storage, Dimensions, Type, MaxObjects, Depth>::
    Tree(AABB<Dimensions, Type> aabb)
        : _aabb(aabb), _size(0), _leaf(true) {
    }

    template<typename Storage, size_t Dimensions, typename Type, size_t
        MaxObjects, size_t Depth>
    std::vector<TreeContent<Storage>>&
    Tree<Storage, Dimensions, Type, MaxObjects, Depth>::getStorage() {
        return _contents;
    }

    template<typename Storage, size_t Dimensions, typename Type, size_t
        MaxObjects, size_t Depth>
    const std::vector<TreeContent<Storage>>&
    Tree<Storage, Dimensions, Type, MaxObjects, Depth>::getStorage() const {
        return _contents;
    }

    template<typename Storage, size_t Dimensions, typename Type, size_t
        MaxObjects, size_t Depth>
    std::vector<AbstractTree<Storage>*>
    Tree<Storage, Dimensions, Type, MaxObjects, Depth>::getChildren() const {
        if constexpr (Depth > 0) {
            if (_leaf) return {};

            std::vector<AbstractTree<Storage>*> v;
            for (auto& child: *_children) {
                v.push_back(&child);
            }
            return v;
        }
        return {};
    }

    template<typename Storage, size_t Dimensions, typename Type, size_t
        MaxObjects, size_t Depth>
    std::any Tree<Storage, Dimensions, Type, MaxObjects, Depth>::
    getBounds() const {
        return _aabb;
    }


    template<typename Storage, size_t Dimensions, typename Type,
        size_t MaxObjects, size_t Depth>
    size_t Tree<Storage, Dimensions, Type, MaxObjects, Depth>::size() const {
        if (_leaf) {
            return _contents.size();
        }

        size_t amount = 0;

        if constexpr (Depth > 0) {
            for (auto& child: *_children) {
                amount += child.size();
            }
        }

        return amount;
    }


    template<typename Storage, size_t Dimensions, typename Type, size_t
        MaxObjects, size_t Depth>
    TreeInsertResult Tree<Storage, Dimensions, Type, MaxObjects, Depth>::
    insert(const Storage& storage, const std::any& bounds) {
        if (!rush::intersectsAny<decltype(_aabb), Dimensions, Type>(
            _aabb, bounds)) {
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

    template<typename Storage, size_t Dimensions, typename Type, size_t
        MaxObjects, size_t Depth>
    bool Tree<Storage, Dimensions, Type, MaxObjects, Depth>::remove(
        const Storage& storage) {
        if (_leaf) {
            bool removed = std::erase_if(
                               _contents,
                               [storage](const TreeContent<Storage>& it) {
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
                        [&s](TreeContent<Storage>& item) {
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

    template<typename Storage, size_t Dimensions, typename Type, size_t
        MaxObjects, size_t Depth>
    bool Tree<Storage, Dimensions, Type, MaxObjects, Depth>::contains(
        const Storage& storage) {
        if (_leaf) {
            return std::any_of(
                _contents.begin(),
                _contents.end(),
                [&storage](TreeContent<Storage>& item) {
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

    template<typename Storage, size_t Dimensions, typename Type,
        size_t MaxObjects, size_t Depth>
    bool Tree<Storage, Dimensions, Type, MaxObjects, Depth>::isLeaf() const {
        return _leaf;
    }

    template<typename Storage, size_t Dimensions, typename Type, size_t
        MaxObjects, size_t Depth>
    TreeIterator<Storage> Tree<Storage, Dimensions, Type, MaxObjects, Depth>::
    begin() {
        return TreeIterator<Storage>(this);
    }

    template<typename Storage, size_t Dimensions, typename Type, size_t
        MaxObjects, size_t Depth>
    TreeIterator<Storage>
    Tree<Storage, Dimensions, Type, MaxObjects, Depth>::
    end() {
        return TreeIterator<Storage>();
    }

    template<typename Storage>
    TreeIterator<Storage>::TreeIterator(AbstractTree<Storage>* root)
        : _root(root),
          _current(root),
          _end(false) {
        if (_current->getStorage().empty()) {
            operator++();
        }
    }

    template<typename Storage>
    TreeIterator<Storage>::TreeIterator()
        : _root(nullptr),
          _current(nullptr),
          _end(true) {
    }

    template<typename Storage>
    AbstractTree<Storage>*
    TreeIterator<Storage>::getTree() const {
        return _current;
    }


    template<typename Storage>
    const typename TreeIterator<Storage>::reference
    TreeIterator<Storage>::operator*() const {
        return _current->getStorage();
    }

    template<typename Storage>
    typename TreeIterator<Storage>::pointer
    TreeIterator<Storage>::operator->() const {
        return &_current->getStorage();
    }

    template<typename Storage>
    TreeIterator<Storage>&
    TreeIterator<Storage>::operator++() {
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

    template<typename Storage>
    TreeIterator<Storage>
    TreeIterator<Storage>::operator++(Storage) {
        auto tmp = *this;
        ++(*this);
        return tmp;
    }

    template<typename Storage>
    bool TreeIterator<Storage>::operator==(const TreeIterator& p) {
        return (_end && p._end) || _current == p._current;
    }

    template<typename Storage>
    bool TreeIterator<Storage>::operator!=(const TreeIterator& p) {
        return !((_end && p._end) || _current == p._current);
    }
}
