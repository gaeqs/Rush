//
// Created by gaelr on 29/01/2024.
//

#ifndef TREE_IMPL_H
#define TREE_IMPL_H

namespace rush {
    template<typename Storage, typename Bounds>
    bool TreeContent<Storage, Bounds>::operator==(const TreeContent& o) const {
        return storage == o.storage;
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
    typename TreeIterator<TreeBounds, Storage, Bounds, FilterBounds>::reference
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

template<typename Storage, typename Bounds>
struct std::hash<rush::TreeContent<Storage, Bounds>> {
    std::size_t
    operator()(const rush::TreeContent<Storage, Bounds>& s) const noexcept {
        std::hash<Storage> hasher;
        return hasher(s.storage);
    }
};

#endif //TREE_IMPL_H