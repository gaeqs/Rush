//
// Created by gaeqs on 24/01/2025.
//

#ifndef MATRIX_LU_DECOMPOSE_H
#define MATRIX_LU_DECOMPOSE_H

#include <rush/matrix/mat_sparse_rep.h>

namespace rush {
    template<size_t Columns, size_t Rows, typename Type, typename Allocator, typename OAlloc>
    bool sparseLUDecompose(const Mat<Columns, Rows, Type, MatSparseRep, Allocator>& matrix,
                           Mat<Columns, Rows, Type, MatSparseRep, OAlloc>& out) {
        constexpr Type ZERO = static_cast<Type>(0);

        Vec<Columns, std::vector<std::pair<size_t, Type>>> tempLU;
        for (auto& entry: tempLU) entry.reserve(Columns);
        size_t amount = 0;

        for (size_t col = 0; col < Columns; ++col) {
            Vec<Columns, Type> columnValues;
            for (int idx = matrix.rep.cols[col]; idx < matrix.rep.cols[col + 1]; ++idx) {
                columnValues[matrix.rep.rows[idx]] = matrix.rep.vals[idx];
            }

            for (size_t i = 0; i < col; ++i) {
                Type factor = columnValues[i];
                if (factor != ZERO) {
                    for (const auto& [row, val]: tempLU[i]) {
                        if (row > i) {
                            columnValues[row] -= factor * val;
                        }
                    }
                }
            }

            for (size_t i = 0; i < Columns; ++i) {
                if (columnValues[i] != ZERO) {
                    if (i <= col) {
                        tempLU[col].emplace_back(i, columnValues[i]); // U
                    } else {
                        if (columnValues[col] == ZERO) return false;
                        tempLU[col].emplace_back(i, columnValues[i] / columnValues[col]); // L
                    }
                    ++amount;
                }
            }
        }

        out.rep.rows.reserve(amount);
        out.rep.vals.reserve(amount);
        for (int col = 0; col < Columns; ++col) {
            out.rep.cols[col + 1] = out.rep.cols[col] + tempLU[col].size();
            for (const auto& [row, val]: tempLU[col]) {
                out.rep.rows.push_back(row);
                out.rep.vals.push_back(val);
            }
        }

        return true;
    }

    template<size_t Columns, size_t Rows, typename Type, typename Representation,
        typename Allocator, typename ORep, typename OAlloc>
    bool genericLUDecompose(const Mat<Columns, Rows, Type, Representation, Allocator>& matrix,
                            Mat<Columns, Rows, Type, ORep, OAlloc>& out) {
        for (size_t i = 0; i < Columns; i++) {
            for (size_t j = i; j < Columns; j++) {
                Type sum = static_cast<Type>(0);

                for (size_t k = 0; k < i; ++k) {
                    sum += out(k, i) * out(j, k);
                }

                out.pushValue(j, i, matrix(j, i) - sum);
            }


            for (size_t j = i + 1; j < Columns; j++) {
                Type sum = static_cast<Type>(0);

                for (size_t k = 0; k < i; ++k) {
                    sum += out(k, j) * out(i, k);
                }
                Type diagonal = out(i, i);
                if (diagonal == static_cast<Type>(0)) return false;
                Type result = static_cast<Type>(1) / diagonal * (matrix(i, j) - sum);
                out.pushValue(i, j, result);
            }
        }

        return true;
    }
}

#endif //MATRIX_LU_DECOMPOSE_H
