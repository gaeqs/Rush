//
// Created by gaelr on 29/01/2024.
//

#ifndef TREE_BASE_H
#define TREE_BASE_H

#include <array>
#include <vector>
#include <memory>

namespace rush {

    enum class TreeInsertResult {
        ADDED, REMOVED, UPDATED, NOTHING
    };

    template<typename Storage, typename Bounds>
    class TreeIterator;

    template<typename Storage, typename Bounds>
    struct TreeContent {
        Bounds bounds;
        Storage storage;

        bool operator==(const TreeContent& o) const;
    };

    template<typename Storage, typename Bounds>
    class AbstractTree {
    public:
        virtual ~AbstractTree() = default;

        [[nodiscard]] virtual const std::vector<TreeContent<Storage, Bounds>>&
        getStorage() const = 0;

        [[nodiscard]] virtual std::vector<TreeContent<Storage, Bounds>>&
        getStorage() = 0;

        virtual std::vector<AbstractTree*> getChildren() const = 0;

        virtual std::any getBounds() const = 0;

        [[nodiscard]] virtual size_t size() const = 0;
    };

    template<typename Storage, typename Bounds, size_t Dimensions, typename Type,
        size_t MaxObjects, size_t Depth>
    class Tree : public AbstractTree<Storage, Bounds> {
        using ChildType = std::conditional_t<(Depth > 0),
            Tree<Storage, Bounds, Dimensions, Type, MaxObjects, Depth - 1>,
            void*>;

        friend class Tree<Storage, Bounds, Dimensions, Type, MaxObjects, Depth + 1>;

        AABB<Dimensions, Type> _aabb;
        std::unique_ptr<std::array<ChildType, 1 << Dimensions>> _children;
        std::vector<TreeContent<Storage, Bounds>> _contents;
        size_t _size;
        bool _leaf;

        void split();

    public:

        Tree();

        explicit Tree(AABB<Dimensions, Type> aabb);

        ~Tree() override = default;

        [[nodiscard]] const std::vector<TreeContent<Storage, Bounds>>&
        getStorage() const override;

        [[nodiscard]] std::vector<TreeContent<Storage, Bounds>>&
        getStorage() override;

        std::vector<AbstractTree<Storage, Bounds>*> getChildren() const override;

        std::any getBounds() const override;

        [[nodiscard]] size_t size() const override;

        TreeInsertResult insert(const Storage& storage, const Bounds& bounds);

        bool remove(const Storage& storage);

        bool contains(const Storage& storage);

        [[nodiscard]]
        bool isLeaf() const;

        TreeIterator<Storage, Bounds> begin();

        TreeIterator<Storage, Bounds> end();
    };

    template<typename Storage, typename Bounds>
    class TreeIterator {
        using iterator_category = std::forward_iterator_tag;
        using difference_type = std::ptrdiff_t;
        using value_type = std::vector<TreeContent<Storage, Bounds>>;
        using pointer = const value_type*;
        using reference = const value_type&;

        AbstractTree<Storage, Bounds>* _root;
        AbstractTree<Storage, Bounds>* _current;
        std::vector<AbstractTree<Storage, Bounds>*> _queue;
        bool _end;

    public:
        explicit TreeIterator(AbstractTree<Storage, Bounds>* root);

        explicit TreeIterator();

        AbstractTree<Storage, Bounds>* getTree() const;

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
