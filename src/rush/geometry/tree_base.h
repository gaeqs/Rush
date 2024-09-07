//
// Created by gaelr on 29/01/2024.
//

#ifndef TREE_BASE_H
#define TREE_BASE_H

#include <array>
#include <vector>
#include <rush/allocator/pool.h>

namespace rush {

    template<typename TreeBounds, typename Storage, typename Bounds,
            typename FilterBounds = TreeBounds>
    class TreeIterator;

    template<typename Storage, typename Bounds>
    struct TreeContent {
        Bounds bounds;
        Storage storage;

        bool operator==(const TreeContent& o) const;
    };

    template<typename TreeBounds, typename Storage, typename Bounds>
    class AbstractTree {
    public:

        using Content = TreeContent<Storage, Bounds>;

        virtual ~AbstractTree() = default;

        [[nodiscard]] virtual const std::vector<TreeContent<Storage, Bounds>>&
        getStorage() const = 0;

        [[nodiscard]] virtual std::vector<TreeContent<Storage, Bounds>>&
        getStorage() = 0;

        virtual std::vector<AbstractTree*> getChildren() const = 0;

        virtual const TreeBounds& getBounds() const = 0;

        [[nodiscard]] virtual size_t size() const = 0;
    };

    /**
     * Useful struct used to iterate all children of a tree.
     *
     * You can set up bounds to filter the elements
     * so you will only get the leaf nodes the collides with the
     * given bounds.
     *
     * @tparam TreeBounds the type of the bounds used by the tree's nodes.
     * @tparam Storage the storage type of the tree.
     * @tparam Bounds the type of the bounds of the elements inside the tree.
     * @tparam FilterBounds the type of the optional bounds used to filter.
     */
    template<typename TreeBounds, typename Storage, typename Bounds,
            typename FilterBounds>
    class TreeIterator {
        using iterator_category = std::forward_iterator_tag;
        using difference_type = std::ptrdiff_t;
        using value_type = std::vector<TreeContent<Storage, Bounds> >;
        using pointer = const value_type*;
        using reference = const value_type&;

        AbstractTree<TreeBounds, Storage, Bounds>* _root;
        AbstractTree<TreeBounds, Storage, Bounds>* _current;
        std::vector<AbstractTree<TreeBounds, Storage, Bounds>*> _queue;
        std::optional<FilterBounds> _filter;
        bool _end;

    public:
        explicit TreeIterator(AbstractTree<TreeBounds, Storage, Bounds>* root);

        TreeIterator(AbstractTree<TreeBounds, Storage, Bounds>* root,
                     const FilterBounds& filter);

        explicit TreeIterator();

        AbstractTree<TreeBounds, Storage, Bounds>* getTree() const;

        reference operator*() const;

        pointer operator->() const;

        TreeIterator& operator++();

        TreeIterator operator++(int);

        bool operator==(const TreeIterator& p);

        bool operator!=(const TreeIterator& p);
    };
}

#include <rush/geometry/tree_impl.h>

#endif //TREE_BASE_H
