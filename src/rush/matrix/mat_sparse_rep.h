//
// Created by gaeqs on 7/09/24.
//

#ifndef MAT_SPARSE_REP_H
#define MAT_SPARSE_REP_H

#include <rush/vector/vec.h>

namespace rush {
    struct MatSparseRep {
        template<size_t Columns, size_t Rows, typename Type>
        struct Representation {
            using ColumnType = Vec<Rows, Type>;
            using RowType = Vec<Columns, Type>;

            using ColumnRef = Vec<Rows, Type>&;
            using RowRef = VecRef<Columns, Type>;

            std::vector<Type> vals;
            std::vector<size_t> rows;
            std::vector<size_t> cols = std::vector<size_t>(Columns + 1, 0);

            const Type& value(size_t column, size_t row) const {
                size_t colStart = cols[column];
                size_t colEnd = cols[column + 1];

                for (size_t pos = colStart; pos < colEnd; ++pos) {
                    size_t currentRow = rows[pos];
                    if (currentRow == row) {
                        return vals[pos];
                    }
                }

                return static_cast<Type>(0);
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
                        cols.insert(cols.begin() + pos, column);
                        for (size_t i = column; i <= Columns; ++i) {
                            cols[i] += 1;
                        }
                    }
                } else if (value == static_cast<Type>(0)) {
                    // Remove
                    vals.erase(vals.begin() + pos);
                    cols.erase(cols.begin() + pos);
                    for (size_t i = column; i <= Columns; ++i) {
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

            // ENDREGION
        };
    };
}

#endif //MAT_SPARSE_REP_H
