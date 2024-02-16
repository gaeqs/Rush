//
// Created by gaelr on 29/01/2024.
//

#ifndef TREE_IMPL_H
#define TREE_IMPL_H

#endif //TREE_IMPL_H


namespace rush {
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
        : _aabb(), _leaf(true) {
    }

    template<typename Storage, size_t Dimensions, typename Type,
        size_t MaxObjects, size_t Depth>
    Tree<Storage, Dimensions, Type, MaxObjects, Depth>::
    Tree(rush::AABB<Dimensions, Type> aabb) : _aabb(aabb), _leaf(true) {
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
    void Tree<Storage, Dimensions, Type, MaxObjects, Depth>::
    insert(const Storage& storage, const std::any& bounds) {
        remove(storage);
        if (!rush::intersectsAny<decltype(_aabb), Dimensions, Type>(
            _aabb, bounds))
            return;

        if (_leaf) {
            _contents.push_back({bounds, storage});
            if constexpr (Depth > 0) {
                if (_contents.size() > MaxObjects) {
                    split();
                }
            }
        }
        else {
            if constexpr (Depth > 0) {
                for (auto& child: *_children.get()) {
                    child.insert(storage, bounds);
                }
            }
        }
    }

    template<typename Storage, size_t Dimensions, typename Type, size_t
        MaxObjects, size_t Depth>
    void Tree<Storage, Dimensions, Type, MaxObjects, Depth>::remove(
        const Storage& storage) {
        if (_leaf) {
            std::erase_if(
                _contents,
                [storage](const TreeContent<Storage>& it) {
                    return it.storage == storage;
                }
            );
        }
        else {
            if constexpr (Depth > 0) {
                for (auto& child: *_children) {
                    child.remove(storage);
                }

                if (size() <= MaxObjects) {
                    // Remove all children
                    for (auto& child: *_children) {
                        auto it = child.begin();
                        auto end = child.end();

                        while (it != end) {
                            _contents.push_back({it.currentBounds(), *it});
                            ++it;
                        }
                    }
                    _children = nullptr;
                    _leaf = true;
                }
            }
        }
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
          _currentIterator(root->getStorage().begin()),
          _index(0),
          _end(false) {
    }

    template<typename Storage>
    TreeIterator<Storage>::TreeIterator()
        : _root(nullptr),
          _current(nullptr),
          _currentIterator(),
          _index(0),
          _end(true) {
    }

    template<typename Storage>
    std::any TreeIterator<Storage>::currentBounds() const {
        return _currentIterator->bounds;
    }

    template<typename Storage>
    typename TreeIterator<Storage>::reference
    TreeIterator<Storage>::operator*() const {
        return _currentIterator->storage;
    }

    template<typename Storage>
    typename TreeIterator<Storage>::pointer
    TreeIterator<Storage>::operator->() const {
        return &_currentIterator->storage;
    }

    template<typename Storage>
    TreeIterator<Storage>&
    TreeIterator<Storage>::operator++() {
        if (_end) return *this;
        ++_currentIterator;
        ++_index;

        if (_currentIterator == _current->getStorage().end()) {
            // Add children to queue.
            for (auto child: _current->getChildren()) {
                _queue.push_back(child);
            }

            if (!_queue.empty()) {
                _current = _queue.back();
                _queue.pop_back();
                _currentIterator = _current->getStorage().begin();
            }
            else {
                _end = true;
            }
        }

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
        return (_end && p._end) || _index == p._index;
    }

    template<typename Storage>
    bool TreeIterator<Storage>::operator!=(const TreeIterator& p) {
        return !((_end && p._end) || _index == p._index);
    }
}
