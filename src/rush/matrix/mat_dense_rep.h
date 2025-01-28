//
// Created by gaeqs on 7/09/24.
//

#ifndef MAT_DENSE_REP_H
#define MAT_DENSE_REP_H

#include <rush/vector/vec.h>

namespace rush {
    struct MatDenseRep {
        static constexpr bool PinnedMemory = true;
        static constexpr bool CustomLUDecompose = false;

        template<class Rep, typename T, size_t Rows, size_t MaxValue, bool Reverse>
        class SparseIterator {
            Rep _collection;
            size_t _index;

        public:
            using iterator_category = std::forward_iterator_tag;
            using difference_type = std::ptrdiff_t;
            using value_type = T;

            SparseIterator(Rep collection, size_t index): _collection(collection), _index(index) {
                if (_index < MaxValue && _collection->value(_index) == 0) {
                    this->operator++();
                }
            }

            value_type operator*() const {
                return _collection->value(_index);
            }

            [[nodiscard]] size_t row() const {
                return _index % Rows;
            }

            [[nodiscard]] size_t column() const {
                return _index / Rows;
            }

            void jumpToColumn(size_t column) {
                _index = column * Rows;
                if (_index < MaxValue && _collection->value(_index) == 0) {
                    this->operator++();
                }
            }

            // Prefix increment
            SparseIterator& operator++() {
                do {
                    if constexpr (Reverse) {
                        --_index;
                    } else {
                        ++_index;
                    }
                } while (_index < MaxValue && _collection->value(_index) == 0);
                if (_index >= MaxValue) {
                    _index = std::numeric_limits<size_t>::max();
                }
                return *this;
            }

            // Postfix increment
            SparseIterator operator++(int) {
                SparseIterator tmp = *this;
                ++*this;
                return tmp;
            }

            bool operator==(const SparseIterator& b) const {
                return _collection == b._collection && _index == b._index;
            };

            bool operator!=(const SparseIterator& b) const {
                return _collection != b._collection || _index != b._index;
            };
        };


        template<size_t Columns, size_t Rows, typename Type,
            typename Allocator = StaticAllocator>
        struct Representation {
            using ColumnType = Vec<Rows, Type>;
            using RowType = Vec<Columns, Type>;

            using ColumnRef = Vec<Rows, Type>&;
            using RowRef = VecRef<Columns, Type>;

            using Data = typename Allocator::template AllocatedData<Columns, ColumnType>;

            Data data;

            Type& value(size_t column, size_t row) {
                return data[column][row];
            }

            const Type& value(size_t column, size_t row) const {
                return data[column][row];
            }

            const Type& value(size_t index) const {
                return data[index / Rows][index % Rows];
            }

            void pushValue(size_t column, size_t row, const Type& value) {
                data[column][row] = value;
            }

            RowRef rowRef(size_t row) {
                VecRef<Columns, Type> vec;
                for (size_t i = 0; i < Columns; ++i) {
                    vec.references[i] = &data[i][row];
                }
                return vec;
            }

            RowType row(size_t row) const {
                Vec<Columns, Type> vec;
                for (size_t i = 0; i < Columns; ++i) {
                    vec[i] = data[i][row];
                }
                return vec;
            }

            ColumnRef columnRef(size_t column) {
                return data[column];
            }

            ColumnType column(size_t column) const {
                return data[column];
            }

            Type* toPointer() {
                return data[0].toPointer();
            }

            const Type* toPointer() const {
                return data[0].toPointer();
            }

            // REGION ITERATOR

            auto begin() {
                return data.begin();
            }

            auto end() {
                return data.end();
            }

            auto cbegin() const {
                return data.cbegin();
            }

            auto cend() const {
                return data.cend();
            }

            auto rbegin() {
                return data.rbegin();
            }

            auto rend() {
                return data.rend();
            }

            auto crbegin() const {
                return data.crbegin();
            }

            auto crend() const {
                return data.cbegin();
            }

            auto sparseBegin() const {
                return SparseIterator<const Representation*, Type, Rows, Columns * Rows, false>(
                    this, 0
                );
            }

            auto sparseEnd() const {
                return SparseIterator<const Representation*, Type, Rows, Columns * Rows, false>(
                    this, std::numeric_limits<size_t>::max()
                );
            }

            auto reverseSparseBegin() const {
                return SparseIterator<const Representation*, Type, Rows, Columns * Rows, true>(
                    this, Columns * Rows - 1
                );
            }

            auto reverseSparseEnd() const {
                return SparseIterator<const Representation*, Type, Rows, Columns * Rows, true>(
                    this, std::numeric_limits<size_t>::max()
                );
            }

            // ENDREGION
        };
    };
}

#endif //MAT_DENSE_REP_H
