//
// Created by gaeqs on 7/09/24.
//

#ifndef MAT_DENSE_REP_H
#define MAT_DENSE_REP_H

#include <rush/vector/vec.h>

namespace rush {
    struct MatDenseRep {
        template<size_t Columns, size_t Rows, typename Type,
            typename Allocator = StaticAllocator>
        struct Representation {
            using ColumnType = Vec<Rows, Type>;
            using RowType = Vec<Columns, Type>;

            using ColumnRef = Vec<Rows, Type>&;
            using RowRef = VecRef<Columns, Type>;

            using Data = typename Allocator::template AllocatedData<Columns,
                ColumnType>;

            Data data;

            Type& value(size_t column, size_t row) {
                return data[column][row];
            }

            const Type& value(size_t column, size_t row) const {
                return data[column][row];
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

            // ENDREGION
        };
    };
}

#endif //MAT_DENSE_REP_H
