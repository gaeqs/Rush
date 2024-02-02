//
// Created by gaelr on 29/01/2024.
//

#ifndef TREE_IMPL_H
#define TREE_IMPL_H

#endif //TREE_IMPL_H


namespace rush {
    template<size_t Dimensions, typename Storage, typename Type, size_t
        MaxObjects, size_t Depth>
    void Tree<Dimensions, Storage, Type, MaxObjects, Depth>::split() {
        auto childrenRadius = _aabb.radius / 2;

        _children = std::make_unique<std::array<ChildType, 1 << Dimensions>>();

        for (size_t i = 0; i < _children.size(); ++i) {
            auto localCenter = _aabb.center;
            for (size_t d = 0; d < Dimensions; ++d) {
                if ((i >> d & 1) == 1) {
                    localCenter[d] -= childrenRadius[d];
                }
                else {
                    localCenter[d] += childrenRadius[d];
                }
            }

            (*_children)[i]._aabb = {localCenter, _children};
        }

        for (auto& child: _children) {
            for (auto& content: _contents) {
                child->insert(content.storage, content.bounds);
            }
        }

        _children.clear();

        _leaf = false;
    }

    template<size_t Dimensions, typename Storage, typename Type,
        size_t MaxObjects, size_t Depth>
    Tree<Dimensions, Storage, Type, MaxObjects, Depth>::Tree()
        : _aabb(), _leaf(true) {
    }

    template<size_t Dimensions, typename Storage, typename Type,
        size_t MaxObjects, size_t Depth>
    Tree<Dimensions, Storage, Type, MaxObjects, Depth>::
    Tree(rush::AABB<Dimensions, Type> aabb) : _aabb(aabb), _leaf(true) {
    }

    template<size_t Dimensions, typename Storage, typename Type, size_t
        MaxObjects, size_t Depth>
    void Tree<Dimensions, Storage, Type, MaxObjects, Depth>::
    insert(const Storage& storage, const std::any& bounds) {
        if (!rush::intersectsAny<decltype(_aabb), Dimensions, Type>(
            _aabb, bounds))
            return;

        if (_leaf) {
            _contents.push_back({bounds, storage});
            if (_contents.size() > MaxObjects && Depth > 0) {
                split();
            }
        }
        else {
            for (auto& child: _children) {
                child->insert(storage, bounds);
            }
        }
    }

    template<size_t Dimensions, typename Storage, typename Type,
        size_t MaxObjects, size_t Depth>
    bool Tree<Dimensions, Storage, Type, MaxObjects, Depth>::isLeaf() const {
        return _leaf;
    }
}
