//
// Created by gaelr on 29/01/2024.
//

#ifndef TREE_BASE_H
#define TREE_BASE_H

#include <array>
#include <vector>
#include <memory>

namespace rush {
    template<typename Storage>
    class TreeIterator;

    template<typename Storage>
    struct TreeContent {
        std::any bounds;
        Storage storage;
    };

    template<typename Storage>
    class AbstractTree {
    public:
        virtual ~AbstractTree() = default;

        [[nodiscard]] virtual const std::vector<TreeContent<Storage>>&
        getStorage() const = 0;

        [[nodiscard]] virtual std::vector<TreeContent<Storage>>&
        getStorage() = 0;

        virtual std::vector<AbstractTree*> getChildren() const = 0;

        [[nodiscard]] virtual size_t size() const = 0;
    };

    template<typename Storage, size_t Dimensions, typename Type,
        size_t MaxObjects, size_t Depth>
    class Tree : public AbstractTree<Storage> {
        using ChildType = std::conditional_t<(Depth > 0),
            Tree<Storage, Dimensions, Type, MaxObjects, Depth - 1>,
            void*>;

        friend class Tree<Storage, Dimensions, Type, MaxObjects, Depth + 1>;

        rush::AABB<Dimensions, Type> _aabb;
        std::unique_ptr<std::array<ChildType, 1 << Dimensions>> _children;
        std::vector<TreeContent<Storage>> _contents;
        bool _leaf;

        void split();

    public:

        Tree();

        explicit Tree(rush::AABB<Dimensions, Type> aabb);

        [[nodiscard]] const std::vector<TreeContent<Storage>>&
        getStorage() const override;

        [[nodiscard]] std::vector<TreeContent<Storage>>&
        getStorage() override;

        std::vector<AbstractTree<Storage>*> getChildren() const override;

        [[nodiscard]] size_t size() const override;

        void insert(const Storage& storage, const std::any& bounds);

        void remove(const Storage& storage);

        [[nodiscard]]
        bool isLeaf() const;

        TreeIterator<Storage> begin();

        TreeIterator<Storage> end();
    };

    template<typename Storage>
    class TreeIterator {
        using iterator_category = std::forward_iterator_tag;
        using difference_type = std::ptrdiff_t;
        using value_type = Storage;
        using pointer = Storage*;
        using reference = Storage&;

        AbstractTree<Storage>* _root;
        AbstractTree<Storage>* _current;
        typename std::vector<TreeContent<Storage>>::iterator _currentIterator;
        std::vector<AbstractTree<Storage>*> _queue;
        size_t _index;
        bool _end;

    public:
        explicit TreeIterator(AbstractTree<Storage>* root);

        explicit TreeIterator();

        std::any currentBounds() const;

        reference operator*() const;

        pointer operator->() const;

        TreeIterator& operator++();

        TreeIterator operator++(Storage);

        bool operator==(const TreeIterator& p);

        bool operator!=(const TreeIterator& p);
    };
}

#include <rush/geometry/tree_impl.h>

#endif //TREE_BASE_H
