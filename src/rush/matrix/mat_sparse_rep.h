//
// Created by gaeqs on 7/09/24.
//

#ifndef MAT_SPARSE_REP_H
#define MAT_SPARSE_REP_H

#include <vector>
#include <rush/vector/vec.h>

namespace rush {
    struct MatSparseRep {
        static constexpr bool PinnedMemory = false;

        template<class Rep, typename T, bool Reverse>
        class Iterator {
            Rep _collection;
            size_t _index;

        public:
            using iterator_category = std::forward_iterator_tag;
            using difference_type = std::ptrdiff_t;
            using value_type = T;

            Iterator(Rep collection, size_t index): _collection(collection), _index(index) {}


            value_type operator*() const {
                return _collection->column(_index);
            }

            // Prefix increment
            Iterator& operator++() {
                if constexpr (Reverse) {
                    --_index;
                } else {
                    ++_index;
                }
                return *this;
            }

            // Postfix increment
            Iterator operator++(int) {
                Iterator tmp = *this;
                ++*this;
                return tmp;
            }

            bool operator==(const Iterator& b) const {
                return _collection == b._collection && _index == b._index;
            };

            bool operator!=(const Iterator& b) const {
                return _collection != b._collection || _index != b._index;
            };
        };

        template<class Rep, typename T, bool Reverse>
        class SparseIterator {
            Rep _collection;
            size_t _index;
            size_t _columnMaxIndex;
            size_t _column;

        public:
            using iterator_category = std::forward_iterator_tag;
            using difference_type = std::ptrdiff_t;
            using value_type = T;

            SparseIterator(Rep collection, size_t index)
                : _collection(collection),
                  _index(index),
                  _columnMaxIndex(0),
                  _column(Reverse ? _collection->cols.size() - 2 : 0) {
                computeColumn();
            }


            value_type operator*() const {
                return _collection->vals[_index];
            }

            [[nodiscard]] size_t row() const {
                return _collection->rows[_index];
            }

            [[nodiscard]] size_t column() const {
                return _column;
            }

            void jumpToColumn(size_t column) {
                _index = _collection->cols[column];
                _column = column;
                computeColumn();
            }

            // Prefix increment
            SparseIterator& operator++() {
                if constexpr (Reverse) {
                    --_index;
                    if (_columnMaxIndex > _index) {
                        computeColumn();
                    }
                } else {
                    ++_index;
                    if (_columnMaxIndex <= _index) {
                        computeColumn();
                    }
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
            }

            bool operator!=(const SparseIterator& b) const {
                return _collection != b._collection || _index != b._index;
            }

        private:
            void computeColumn() {
                if constexpr (Reverse) {
                    size_t column = _column;
                    while (column > 0 && _collection->cols[column] > _index) {
                        --column;
                    }
                    _column = column;
                    _columnMaxIndex = _collection->cols[column];
                } else {
                    size_t column = _column + 1;
                    while (_collection->cols.size() > column && _collection->cols[column] <= _index) {
                        ++column;
                    }
                    _column = column - 1;
                    if (_collection->cols.size() > column) {
                        _columnMaxIndex = _collection->cols[column];
                    }
                }
            }
        };

        template<size_t Columns, size_t Rows, typename Type, typename UnusedAllocator = StaticAllocator>
        struct Representation {
            using ColumnType = Vec<Rows, Type>;
            using RowType = Vec<Columns, Type>;

            using ColumnRef = void;
            using RowRef = void;

            std::vector<Type> vals;
            std::vector<size_t> rows;
            Vec<Columns + 1, size_t> cols;

            const Type& value(size_t column, size_t row) const {
                static const Type ZERO = static_cast<Type>(0);
                size_t colStart = cols[column];
                size_t colEnd = cols[column + 1];

                for (size_t pos = colStart; pos < colEnd; ++pos) {
                    size_t currentRow = rows[pos];
                    if (currentRow == row) {
                        return vals[pos];
                    }
                }

                return ZERO;
            }

            void pushValue(size_t column, size_t row, const Type& value) {
                size_t colStart = cols[column];
                size_t colEnd = cols[column + 1];
                size_t pos = colStart;
                size_t currentRow = Rows;
                for (; pos < colEnd; ++pos) {
                    currentRow = rows[pos];
                    if (currentRow >= row) break;
                }

                if (currentRow != row) {
                    if (value != static_cast<Type>(0)) {
                        // Insert
                        vals.insert(vals.begin() + pos, value);
                        rows.insert(rows.begin() + pos, row);
                        for (size_t i = column + 1; i <= Columns; ++i) {
                            cols[i] += 1;
                        }
                    }
                } else if (value == static_cast<Type>(0)) {
                    // Remove
                    vals.erase(vals.begin() + pos);
                    rows.erase(rows.begin() + pos);
                    for (size_t i = column + 1; i <= Columns; ++i) {
                        cols[i] -= 1;
                    }
                } else {
                    vals[pos] = value;
                }
            }

            RowType row(size_t row) const {
                Vec<Columns, Type> vec;
                for (size_t i = 0; i < Columns; ++i) {
                    vec[i] = value(i, row);
                }
                return vec;
            }

            ColumnType column(size_t column) const {
                Vec<Rows, Type> vec;

                size_t colStart = cols[column];
                size_t colEnd = cols[column + 1];
                for (size_t pos = colStart; pos < colEnd; ++pos) {
                    vec[rows[pos]] = vals[pos];
                }

                return vec;
            }

            // REGION ITERATOR

            auto begin() {
                return Iterator<const Representation*, ColumnType, false>(this, 0);
            }

            auto end() {
                return Iterator<const Representation*, ColumnType, false>(this, Columns);
            }

            auto cbegin() const {
                return Iterator<const Representation*, ColumnType, false>(this, 0);
            }

            auto cend() const {
                return Iterator<const Representation*, ColumnType, false>(this, Columns);
            }

            auto rbegin() {
                return Iterator<const Representation*, ColumnType, true>(this, Columns);
            }

            auto rend() {
                return Iterator<const Representation*, ColumnType, true>(this, 0);
            }

            auto crbegin() const {
                return Iterator<const Representation*, ColumnType, true>(this, Columns);
            }

            auto crend() const {
                return Iterator<const Representation*, ColumnType, true>(this, 0);
            }

            auto sparseBegin() const {
                return SparseIterator<const Representation*, Type, false>(this, 0);
            }

            auto sparseEnd() const {
                return SparseIterator<const Representation*, Type, false>(this, vals.size());
            }

            auto reverseSparseBegin() const {
                return SparseIterator<const Representation*, Type, true>(this, vals.size() - 1);
            }

            auto reverseSparseEnd() const {
                return SparseIterator<const Representation*, Type, true>(
                    this,
                    std::numeric_limits<size_t>::max()
                );
            }

            // ENDREGION
        };
    };
}

#endif //MAT_SPARSE_REP_H
