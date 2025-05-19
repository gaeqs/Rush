//
// Created by gaelr on 12/08/2023.
//

#ifndef RUSH_MAT_IMPL_H
#define RUSH_MAT_IMPL_H

#include <rush/quaternion/quat.h>
#include <rush/matrix/matrix_lu_decompose.h>

namespace rush {
    template<size_t Columns, size_t Rows, typename Type, typename Representation, typename Allocator>
    template<typename... T>
        requires (std::is_convertible_v<std::common_type_t<T...>, Type>
                  && sizeof...(T) <= Columns * Rows && sizeof...(T) > 1)
    Mat<Columns, Rows, Type, Representation, Allocator>::Mat(T... list) {
        size_t index = 0;
        std::initializer_list<int>{
            ([&] {
                rep.pushValue(index / Rows, index % Rows, list);
                ++index;
            }(), 0)...
        };
    }

    template<size_t Columns, size_t Rows, typename Type, typename Representation, typename Allocator>
    Mat<Columns, Rows, Type, Representation, Allocator>::Mat() : rep() {}

    template<size_t Columns, size_t Rows, typename Type, typename Representation, typename Allocator>
    Mat<Columns, Rows, Type, Representation,
        Allocator>::Mat(Type diagonal) : rep() {
        for (size_t i = 0; i < std::min(Columns, Rows); ++i) {
            rep.pushValue(i, i, diagonal);
        }
    }

    template<size_t Columns, size_t Rows, typename Type, typename Representation, typename Allocator>
    Mat<Columns, Rows, Type, Representation, Allocator>::Mat(
        std::function<Type(size_t, size_t)> populator) {
        for (size_t c = 0; c < Columns; ++c) {
            for (size_t r = 0; r < Rows; ++r) {
                rep.pushValue(c, r, populator(c, r));
            }
        }
    }

    template<size_t Columns, size_t Rows, typename Type, typename Representation, typename Allocator>
    Mat<Columns, Rows, Type, Representation, Allocator>::Mat(
        std::function<Type(size_t, size_t, size_t, size_t)> populator) {
        for (size_t c = 0; c < Columns; ++c) {
            for (size_t r = 0; r < Rows; ++r) {
                rep.pushValue(c, r, populator(c, r, Columns, Rows));
            }
        }
    }

    template<size_t Columns, size_t Rows, typename Type, typename Representation, typename Allocator>
    template<size_t OColumns, size_t ORows, typename ORep, typename OAlloc> requires(Columns > OColumns || Rows > ORows)
    Mat<Columns, Rows, Type, Representation, Allocator>::Mat(
        const Mat<OColumns, ORows, Type, ORep, OAlloc>& other, Type diagonal) : Mat(diagonal) {
        for (size_t c = 0; c < std::min(Columns, OColumns); ++c) {
            for (size_t r = 0; r < std::min(Rows, ORows); ++r) {
                rep.pushValue(c, r, other[c][r]);
            }
        }
    }

    template<size_t Columns, size_t Rows, typename Type, typename Representation, typename Allocator>
    template<typename ORep, typename OAlloc>
    Mat<Columns, Rows, Type, Representation, Allocator>::Mat(const Mat<Columns, Rows, Type, ORep, OAlloc>& other) {
        for (size_t c = 0; c < Columns; ++c) {
            for (size_t r = 0; r < Rows; ++r) {
                rep.pushValue(c, r, other[c][r]);
            }
        }
    }

    template<size_t Columns, size_t Rows, typename Type, typename Representation, typename Allocator>
    constexpr size_t Mat<Columns, Rows, Type, Representation, Allocator>::size() const {
        return Columns * Rows;
    }

    template<size_t Columns, size_t Rows, typename Type, typename Representation, typename Allocator>
    const Type* Mat<Columns, Rows, Type, Representation,
        Allocator>::toPointer() const requires Representation::PinnedMemory {
        return rep.toPointer();
    }

    template<size_t Columns, size_t Rows, typename Type, typename Representation, typename Allocator>
    Type* Mat<Columns, Rows, Type, Representation, Allocator>::toPointer() requires Representation::PinnedMemory {
        return rep.toPointer();
    }

    template<size_t Columns, size_t Rows, typename Type, typename Representation, typename Allocator>
    typename Mat<Columns, Rows, Type, Representation, Allocator>::Rep::ColumnRef
    Mat<Columns, Rows, Type, Representation, Allocator>::column(size_t column) requires Representation::PinnedMemory {
        return rep.columnRef(column);
    }

    template<size_t Columns, size_t Rows, typename Type, typename Representation, typename Allocator>
    typename Mat<Columns, Rows, Type, Representation, Allocator>::Rep::ColumnType
    Mat<Columns, Rows, Type, Representation, Allocator>::
    column(size_t column) const {
        return rep.column(column);
    }

    template<size_t Columns, size_t Rows, typename Type, typename Representation, typename Allocator>
    typename Mat<Columns, Rows, Type, Representation, Allocator>::Rep::RowRef
    Mat<Columns, Rows, Type, Representation, Allocator>::row(size_t row) requires Representation::PinnedMemory {
        return rep.rowRef(row);
    }

    template<size_t Columns, size_t Rows, typename Type, typename Representation, typename Allocator>
    typename Mat<Columns, Rows, Type, Representation, Allocator>::Rep::RowType
    Mat<Columns, Rows, Type, Representation, Allocator>::row(size_t row) const {
        return rep.row(row);
    }

    template<size_t Columns, size_t Rows, typename Type, typename Representation, typename Allocator>
    typename Mat<Columns, Rows, Type, Representation, Allocator>::Rep::ColumnRef
    Mat<Columns, Rows, Type, Representation, Allocator>::operator[](size_t column) requires
        Representation::PinnedMemory {
        return rep.columnRef(column);
    }

    template<size_t Columns, size_t Rows, typename Type, typename Representation, typename Allocator>
    typename Mat<Columns, Rows, Type, Representation, Allocator>::Rep::ColumnType
    Mat<Columns, Rows, Type, Representation, Allocator>::operator[](size_t column) const {
        return rep.column(column);
    }

    template<size_t Columns, size_t Rows, typename Type, typename Representation, typename Allocator>
    Type& Mat<Columns, Rows, Type, Representation, Allocator>::operator()(size_t column, size_t row)
        requires Representation::PinnedMemory {
        return rep.value(column, row);
    }

    template<size_t Columns, size_t Rows, typename Type, typename Representation
        , typename Allocator>
    const Type& Mat<Columns, Rows, Type, Representation, Allocator>::operator()(size_t column, size_t row) const {
        return rep.value(column, row);
    }

    template<size_t Columns, size_t Rows, typename Type, typename Representation, typename Allocator>
    void Mat<Columns, Rows, Type, Representation, Allocator>::pushValue(size_t column, size_t row, const Type& value) {
        rep.pushValue(column, row, value);
    }

    template<size_t Columns, size_t Rows, typename Type, typename Representation, typename Allocator>
    Type Mat<Columns, Rows, Type, Representation, Allocator>::determinant() const requires
        HasAdd<Type> && HasSub<Type> && HasMul<Type> && HasDiv<Type> && (Columns == Rows) {
        if constexpr (Columns == 1) {
            return rep.value(0, 0);
        } else if constexpr (Columns == 2) {
            return rep.value(0, 0) * rep.value(1, 1)
                   - rep.value(0, 1) * rep.value(1, 0);
        } else if constexpr (Columns == 3) {
            return +rep.value(0, 0) *
                   (rep.value(1, 1) * rep.value(2, 2)
                    - rep.value(1, 2) * rep.value(2, 1))
                   - rep.value(0, 1) *
                   (rep.value(1, 0) * rep.value(2, 2)
                    - rep.value(1, 2) * rep.value(2, 0))
                   + rep.value(0, 2) *
                   (rep.value(1, 0) * rep.value(2, 1)
                    - rep.value(1, 1) * rep.value(2, 0));
        } else {
            // Generic method
            Self tempM = *this;

            Type det = tempM[0][0];
            for (size_t i = 0; i < Columns; i++) {
                size_t pivot = i;
                for (size_t j = i + 1; j < Columns; j++) {
                    if (std::abs(tempM[j][i]) > std::abs(tempM[pivot][i])) {
                        pivot = j;
                    }
                }

                if (pivot != i) {
                    std::swap(tempM[i], tempM[pivot]);
                    det = -det;
                }

                if (i != 0) {
                    det *= tempM[i][i];
                }
                if (tempM[i][i] == 0) return det;

                for (size_t j = i + 1; j < Columns; j++) {
                    Type factor = tempM[j][i] / tempM[i][i];
                    for (size_t k = i + 1; k < Columns; k++) {
                        tempM[j][k] -= factor * tempM[i][k];
                    }
                }
            }
            return det;
        }
    }

    template<size_t Columns, size_t Rows, typename Type, typename Representation
        , typename Allocator>
    Mat<Rows, Columns, Type, Representation, Allocator>
    Mat<Columns, Rows, Type, Representation, Allocator>::transpose() const {
        if constexpr (std::is_same_v<Representation, MatSparseRep>) {
            Mat transposed;
            transposed.rep.vals.resize(rep.vals.size());
            transposed.rep.rows.resize(rep.vals.size());

            for (size_t i = 0; i < rep.vals.size(); i++) {
                ++transposed.rep.cols[rep.rows[i] + 1];
            }

            for (size_t i = 1; i <= Columns; ++i) {
                transposed.rep.cols[i] += rep.cols[i - 1];
            }

            auto positions = transposed.rep.cols;
            for (size_t col = 0; col < Columns; ++col) {
                for (size_t idx = rep.cols[col]; idx < rep.cols[col + 1]; ++idx) {
                    size_t row = rep.rows[idx];
                    Type value = rep.vals[idx];

                    size_t pos = positions[row]++;
                    transposed.rep.vals[pos] = value;
                    transposed.rep.rows[pos] = col;
                }
            }

            return transposed;
        } else {
            return Mat<Rows, Columns, Type, Representation, Allocator>
            ([this](size_t c, size_t r) {
                return operator()(r, c);
            });
        }
    }

    template<size_t Columns, size_t Rows, typename Type, typename Representation
        , typename Allocator>
    Mat<Columns, Rows, Type, Representation, Allocator>::Self
    Mat<Columns, Rows, Type, Representation, Allocator>::inverse() const
        requires HasAdd<Type> && HasSub<Type> && HasMul<Type> && HasDiv<Type> && (Columns == Rows) && (Columns < 5) {
        auto& d = *this;
        if constexpr (Columns == 1) {
            return {Type(1) / d[0][0]};
        } else if constexpr (Columns == 2) {
            Type det = determinant();
            return {d[1][1] / det, -d[0][1] / det, -d[1][0] / det, d[0][0] / det};
        } else if constexpr (Columns == 3) {
            Type det = determinant();
            Self inverse;
            inverse.pushValue(0, 0, +(d[1][1] * d[2][2] - d[2][1] * d[1][2]));
            inverse.pushValue(1, 0, -(d[1][0] * d[2][2] - d[2][0] * d[1][2]));
            inverse.pushValue(2, 0, +(d[1][0] * d[2][1] - d[2][0] * d[1][1]));
            inverse.pushValue(0, 1, -(d[0][1] * d[2][2] - d[2][1] * d[0][2]));
            inverse.pushValue(1, 1, +(d[0][0] * d[2][2] - d[2][0] * d[0][2]));
            inverse.pushValue(2, 1, -(d[0][0] * d[2][1] - d[2][0] * d[0][1]));
            inverse.pushValue(0, 2, +(d[0][1] * d[1][2] - d[1][1] * d[0][2]));
            inverse.pushValue(1, 2, -(d[0][0] * d[1][2] - d[1][0] * d[0][2]));
            inverse.pushValue(2, 2, +(d[0][0] * d[1][1] - d[1][0] * d[0][1]));
            return inverse / det;
        } else {
            // Based of https://github.com/g-truc/glm/blob/master/glm/gtc/matrix_inverse.inl
            Type s00 = d[2][2] * d[3][3] - d[3][2] * d[2][3];
            Type s01 = d[2][1] * d[3][3] - d[3][1] * d[2][3];
            Type s02 = d[2][1] * d[3][2] - d[3][1] * d[2][2];
            Type s03 = d[2][0] * d[3][3] - d[3][0] * d[2][3];
            Type s04 = d[2][0] * d[3][2] - d[3][0] * d[2][2];
            Type s05 = d[2][0] * d[3][1] - d[3][0] * d[2][1];
            Type s06 = d[1][2] * d[3][3] - d[3][2] * d[1][3];
            Type s07 = d[1][1] * d[3][3] - d[3][1] * d[1][3];
            Type s08 = d[1][1] * d[3][2] - d[3][1] * d[1][2];
            Type s09 = d[1][0] * d[3][3] - d[3][0] * d[1][3];
            Type s10 = d[1][0] * d[3][2] - d[3][0] * d[1][2];
            Type s11 = d[1][0] * d[3][1] - d[3][0] * d[1][1];
            Type s12 = d[1][2] * d[2][3] - d[2][2] * d[1][3];
            Type s13 = d[1][1] * d[2][3] - d[2][1] * d[1][3];
            Type s14 = d[1][1] * d[2][2] - d[2][1] * d[1][2];
            Type s15 = d[1][0] * d[2][3] - d[2][0] * d[1][3];
            Type s16 = d[1][0] * d[2][2] - d[2][0] * d[1][2];
            Type s17 = d[1][0] * d[2][1] - d[2][0] * d[1][1];

            Self inv;
            inv.pushValue(0, 0, +(d[1][1] * s00 - d[1][2] * s01 + d[1][3] * s02));
            inv.pushValue(1, 0, -(d[1][0] * s00 - d[1][2] * s03 + d[1][3] * s04));
            inv.pushValue(2, 0, +(d[1][0] * s01 - d[1][1] * s03 + d[1][3] * s05));
            inv.pushValue(3, 0, -(d[1][0] * s02 - d[1][1] * s04 + d[1][2] * s05));
            inv.pushValue(0, 1, -(d[0][1] * s00 - d[0][2] * s01 + d[0][3] * s02));
            inv.pushValue(1, 1, +(d[0][0] * s00 - d[0][2] * s03 + d[0][3] * s04));
            inv.pushValue(2, 1, -(d[0][0] * s01 - d[0][1] * s03 + d[0][3] * s05));
            inv.pushValue(3, 1, +(d[0][0] * s02 - d[0][1] * s04 + d[0][2] * s05));
            inv.pushValue(0, 2, +(d[0][1] * s06 - d[0][2] * s07 + d[0][3] * s08));
            inv.pushValue(1, 2, -(d[0][0] * s06 - d[0][2] * s09 + d[0][3] * s10));
            inv.pushValue(2, 2, +(d[0][0] * s07 - d[0][1] * s09 + d[0][3] * s11));
            inv.pushValue(3, 2, -(d[0][0] * s08 - d[0][1] * s10 + d[0][2] * s11));
            inv.pushValue(0, 3, -(d[0][1] * s12 - d[0][2] * s13 + d[0][3] * s14));
            inv.pushValue(1, 3, +(d[0][0] * s12 - d[0][2] * s15 + d[0][3] * s16));
            inv.pushValue(2, 3, -(d[0][0] * s13 - d[0][1] * s15 + d[0][3] * s17));
            inv.pushValue(3, 3, +(d[0][0] * s14 - d[0][1] * s16 + d[0][2] * s17));

            Type det = d[0][0] * inv[0][0] + d[0][1] * inv[1][0]
                       + d[0][2] * inv[2][0] + d[0][3] * inv[3][0];

            return inv / det;
        }
    }

    template<size_t Columns, size_t Rows, typename Type, typename Representation, typename Allocator>
    std::pair<bool, typename Mat<Columns, Rows, Type, Representation, Allocator>::Self>
    Mat<Columns, Rows, Type, Representation, Allocator>::luDecomposed() const requires
        HasAdd<Type> && HasSub<Type> && HasMul<Type> && HasDiv<Type> && (Columns == Rows) {
        Self decomposed;
        bool result = luDecomposed(decomposed);
        return std::make_pair(result, decomposed);
    }

    template<size_t Columns, size_t Rows, typename Type, typename Representation, typename Allocator>
    template<typename ORep, typename OAlloc>
    bool Mat<Columns, Rows, Type, Representation, Allocator>::luDecomposed(
        Mat<Columns, Rows, Type, ORep, OAlloc>& out) const requires
        HasAdd<Type> && HasSub<Type> && HasMul<Type> && HasDiv<Type> && (Columns == Rows) {
        if constexpr (std::is_same_v<ORep, MatSparseRep> && std::is_same_v<Representation, MatSparseRep>) {
            return sparseLUDecompose(*this, out);
        }

        return genericLUDecompose(*this, out);
    }

    template<size_t Columns, size_t Rows, typename Type, typename Representation, typename Allocator>
    Vec<Rows, Type> Mat<Columns, Rows, Type, Representation, Allocator>::solveLu(const Vec<Rows, Type>& r) {
        Vec<Rows, Type> x, y;

        for (size_t i = 0; i < Columns; i++) {
            Type sum = static_cast<Type>(0);
            for (size_t k = 0; k < i; ++k) {
                sum += operator()(k, i) * y[k];
            }
            y[i] = r[i] - sum;
        }

        for (size_t j = Columns; j > 0; --j) {
            size_t i = j - 1;
            Type sum = static_cast<Type>(0);
            for (size_t k = i + 1; k < Columns; ++k) {
                sum += operator()(k, i) * x[k];
            }
            x[i] = static_cast<Type>(1) / operator()(i, i) * (y[i] - sum);
        }

        return x;
    }

    template<size_t Columns, size_t Rows, typename Type, typename Representation
        , typename Allocator>
    template<typename To, typename ORep, typename OAlloc>
    Mat<Columns, Rows, To, ORep, OAlloc>
    Mat<Columns, Rows, Type, Representation, Allocator>::cast() const {
        return Mat<Columns, Rows, To, ORep, OAlloc>([this](size_t c, size_t r) {
            return static_cast<To>(this->operator()(c, r));
        });
    }

    template<size_t Columns, size_t Rows, typename Type, typename Representation
        , typename Allocator>
    typename Mat<Columns, Rows, Type, Representation, Allocator>::Self&
    Mat<Columns, Rows, Type, Representation, Allocator>::operator+() {
        return *this;
    }

    template<size_t Columns, size_t Rows, typename Type, typename Representation
        , typename Allocator>
    typename Mat<Columns, Rows, Type, Representation, Allocator>::Self&
    Mat<Columns, Rows, Type, Representation, Allocator>::operator+() const {
        return *this;
    }

    template<size_t Columns, size_t Rows, typename Type, typename Representation
        , typename Allocator>
    Mat<Columns, Rows, Type, Representation, Allocator>::Self
    Mat<Columns, Rows, Type, Representation, Allocator>::operator-() const
        requires HasSub<Type> {
        Self result;

        auto it = sparseBegin();
        auto end = sparseEnd();

        while (it != end) {
            result.pushValue(it.column(), it.row(), -*it);
            ++it;
        }

        return result;
    }

    template<size_t Columns, size_t Rows, typename Type, typename Representation
        , typename Allocator>
    typename Mat<Columns, Rows, Type, Representation, Allocator>::Self&
    Mat<Columns, Rows, Type, Representation, Allocator>::operator+=(
        const Type& s) requires HasAdd<Type> {
        for (size_t c = 0; c < Columns; ++c) {
            for (size_t r = 0; r < Rows; ++r) {
                pushValue(c, r, operator()(c, r) + s);
            }
        }
        return *this;
    }

    template<size_t Columns, size_t Rows, typename Type, typename Representation
        , typename Allocator>
    typename Mat<Columns, Rows, Type, Representation, Allocator>::Self&
    Mat<Columns, Rows, Type, Representation, Allocator>::operator-=(
        const Type& s) requires HasSub<Type> {
        for (size_t c = 0; c < Columns; ++c) {
            for (size_t r = 0; r < Rows; ++r) {
                pushValue(c, r, operator()(c, r) - s);
            }
        }
        return *this;
    }

    template<size_t Columns, size_t Rows, typename Type, typename Representation
        , typename Allocator>
    typename Mat<Columns, Rows, Type, Representation, Allocator>::Self&
    Mat<Columns, Rows, Type, Representation, Allocator>::operator*=(
        const Type& s) requires HasMul<Type> {
        for (size_t c = 0; c < Columns; ++c) {
            for (size_t r = 0; r < Rows; ++r) {
                pushValue(c, r, operator()(c, r) * s);
            }
        }
        return *this;
    }

    template<size_t Columns, size_t Rows, typename Type, typename Representation
        , typename Allocator>
    typename Mat<Columns, Rows, Type, Representation, Allocator>::Self&
    Mat<Columns, Rows, Type, Representation, Allocator>::operator/=(
        const Type& s) requires HasDiv<Type> {
        for (size_t c = 0; c < Columns; ++c) {
            for (size_t r = 0; r < Rows; ++r) {
                pushValue(c, r, operator()(c, r) / s);
            }
        }
        return *this;
    }

    template<size_t Columns, size_t Rows, typename Type, typename Representation
        , typename Allocator>
    typename Mat<Columns, Rows, Type, Representation, Allocator>::Self&
    Mat<Columns, Rows, Type, Representation, Allocator>::operator<<=(
        const Type& s) requires HasShl<Type> {
        for (size_t c = 0; c < Columns; ++c) {
            for (size_t r = 0; r < Rows; ++r) {
                pushValue(c, r, operator()(c, r) << s);
            }
        }
        return *this;
    }

    template<size_t Columns, size_t Rows, typename Type, typename Representation
        , typename Allocator>
    typename Mat<Columns, Rows, Type, Representation, Allocator>::Self&
    Mat<Columns, Rows, Type, Representation, Allocator>::operator>>=(
        const Type& s) requires HasShr<Type> {
        for (size_t c = 0; c < Columns; ++c) {
            for (size_t r = 0; r < Rows; ++r) {
                pushValue(c, r, operator()(c, r) >> s);
            }
        }
        return *this;
    }

    template<size_t Columns, size_t Rows, typename Type, typename Representation
        , typename Allocator>
    typename Mat<Columns, Rows, Type, Representation, Allocator>::Self&
    Mat<Columns, Rows, Type, Representation, Allocator>::operator&=(
        const Type& s) requires HasBitAnd<Type> {
        for (size_t c = 0; c < Columns; ++c) {
            for (size_t r = 0; r < Rows; ++r) {
                pushValue(c, r, operator()(c, r) & s);
            }
        }
        return *this;
    }

    template<size_t Columns, size_t Rows, typename Type, typename Representation
        , typename Allocator>
    typename Mat<Columns, Rows, Type, Representation, Allocator>::Self&
    Mat<Columns, Rows, Type, Representation, Allocator>::operator|=(
        const Type& s) requires HasBitOr<Type> {
        for (size_t c = 0; c < Columns; ++c) {
            for (size_t r = 0; r < Rows; ++r) {
                pushValue(c, r, operator()(c, r) | s);
            }
        }
        return *this;
    }

    template<size_t Columns, size_t Rows, typename Type, typename Representation
        , typename Allocator>
    typename Mat<Columns, Rows, Type, Representation, Allocator>::Self&
    Mat<Columns, Rows, Type, Representation, Allocator>::operator^=(
        const Type& s) requires HasBitXor<Type> {
        for (size_t c = 0; c < Columns; ++c) {
            for (size_t r = 0; r < Rows; ++r) {
                pushValue(c, r, operator()(c, r) ^ s);
            }
        }
        return *this;
    }

    template<size_t Columns, size_t Rows, typename Type, typename Representation
        , typename Allocator>
    template<typename OAlloc>
    Mat<Columns, Rows, Type, Representation, Allocator>&
    Mat<Columns, Rows, Type, Representation, Allocator>::operator+=(
        const Mat<Columns, Rows, Type, OAlloc>& o) requires HasAdd<Type> {
        for (size_t c = 0; c < Columns; ++c) {
            for (size_t r = 0; r < Rows; ++r) {
                pushValue(c, r, operator()(c, r) + o(c, r));
            }
        }
        return *this;
    }

    template<size_t Columns, size_t Rows, typename Type, typename Representation
        , typename Allocator>
    template<typename OAlloc>
    Mat<Columns, Rows, Type, Representation, Allocator>&
    Mat<Columns, Rows, Type, Representation, Allocator>::operator-=(
        const Mat<Columns, Rows, Type, OAlloc>& o) requires HasSub<Type> {
        for (size_t c = 0; c < Columns; ++c) {
            for (size_t r = 0; r < Rows; ++r) {
                pushValue(c, r, operator()(c, r) - o(c, r));
            }
        }
        return *this;
    }

    template<size_t Columns, size_t Rows, typename Type, typename Representation
        , typename Allocator>
    Mat<Columns, Rows, Type, Representation, Allocator>::Self
    Mat<Columns, Rows, Type, Representation, Allocator>::operator+(
        const Type& s) const requires
        HasAdd<Type> {
        Self result;

        for (size_t c = 0; c < Columns; ++c) {
            for (size_t r = 0; r < Rows; ++r) {
                result.pushValue(c, r, operator()(c, r) + s);
            }
        }

        return result;
    }

    template<size_t Columns, size_t Rows, typename Type, typename Representation
        , typename Allocator>
    Mat<Columns, Rows, Type, Representation, Allocator>::Self
    Mat<Columns, Rows, Type, Representation, Allocator>::operator-(
        const Type& s) const requires
        HasSub<Type> {
        Self result;

        for (size_t c = 0; c < Columns; ++c) {
            for (size_t r = 0; r < Rows; ++r) {
                result.pushValue(c, r, operator()(c, r) - s);
            }
        }

        return result;
    }

    template<size_t Columns, size_t Rows, typename Type, typename Representation
        , typename Allocator>
    Mat<Columns, Rows, Type, Representation, Allocator>::Self
    Mat<Columns, Rows, Type, Representation, Allocator>::operator*(
        const Type& s) const requires
        HasMul<Type> {
        Self result;

        auto it = sparseBegin();
        auto end = sparseEnd();

        while (it != end) {
            result.pushValue(it.column(), it.row(), *it * s);
            ++it;
        }

        return result;
    }

    template<size_t Columns, size_t Rows, typename Type, typename Representation
        , typename Allocator>
    Mat<Columns, Rows, Type, Representation, Allocator>::Self
    Mat<Columns, Rows, Type, Representation, Allocator>::operator/(
        const Type& s) const requires
        HasDiv<Type> {
        Self result;

        auto it = sparseBegin();
        auto end = sparseEnd();

        while (it != end) {
            result.pushValue(it.column(), it.row(), *it / s);
            ++it;
        }

        return result;
    }

    template<size_t Columns, size_t Rows, typename Type, typename Representation
        , typename Allocator>
    Mat<Columns, Rows, Type, Representation, Allocator>::Self
    Mat<Columns, Rows, Type, Representation, Allocator>::operator<<(
        const Type& s) const requires
        HasShl<Type> {
        Self result;

        for (size_t c = 0; c < Columns; ++c) {
            for (size_t r = 0; r < Rows; ++r) {
                result.pushValue(c, r, operator()(c, r) << s);
            }
        }

        return result;
    }

    template<size_t Columns, size_t Rows, typename Type, typename Representation
        , typename Allocator>
    Mat<Columns, Rows, Type, Representation, Allocator>::Self
    Mat<Columns, Rows, Type, Representation, Allocator>::operator>>(
        const Type& s) const requires
        HasShr<Type> {
        Self result;

        for (size_t c = 0; c < Columns; ++c) {
            for (size_t r = 0; r < Rows; ++r) {
                result.pushValue(c, r, operator()(c, r) >> s);
            }
        }

        return result;
    }

    template<size_t Columns, size_t Rows, typename Type, typename Representation
        , typename Allocator>
    Mat<Columns, Rows, Type, Representation, Allocator>::Self
    Mat<Columns, Rows, Type, Representation, Allocator>::operator&(
        const Type& s) const requires
        HasBitAnd<Type> {
        Self result;

        auto it = sparseBegin();
        auto end = sparseEnd();

        while (it != end) {
            result.pushValue(it.column(), it.row(), *it & s);
            ++it;
        }

        return result;
    }

    template<size_t Columns, size_t Rows, typename Type, typename Representation
        , typename Allocator>
    Mat<Columns, Rows, Type, Representation, Allocator>::Self
    Mat<Columns, Rows, Type, Representation, Allocator>::operator|(
        const Type& s) const requires
        HasBitOr<Type> {
        Self result;

        for (size_t c = 0; c < Columns; ++c) {
            for (size_t r = 0; r < Rows; ++r) {
                result.pushValue(c, r, operator()(c, r) | s);
            }
        }

        return result;
    }

    template<size_t Columns, size_t Rows, typename Type, typename Representation
        , typename Allocator>
    Mat<Columns, Rows, Type, Representation, Allocator>::Self
    Mat<Columns, Rows, Type, Representation, Allocator>::operator^(
        const Type& s) const requires
        HasBitXor<Type> {
        Self result;

        for (size_t c = 0; c < Columns; ++c) {
            for (size_t r = 0; r < Rows; ++r) {
                result.pushValue(c, r, operator()(c, r) ^ s);
            }
        }

        return result;
    }

    template<size_t Columns, size_t Rows, typename Type, typename Representation
        , typename Allocator>
    Mat<Columns, Rows, Type, Representation, Allocator>::Self
    Mat<Columns, Rows, Type, Representation, Allocator>::operator&&(
        const Type& s) const requires
        HasAnd<Type> {
        Self result;

        auto it = sparseBegin();
        auto end = sparseEnd();

        while (it != end) {
            result.pushValue(it.column(), it.row(), *it && s);
            ++it;
        }

        return result;
    }

    template<size_t Columns, size_t Rows, typename Type, typename Representation
        , typename Allocator>
    Mat<Columns, Rows, Type, Representation, Allocator>::Self
    Mat<Columns, Rows, Type, Representation, Allocator>::operator||(
        const Type& s) const requires
        HasOr<Type> {
        Self result;
        for (size_t c = 0; c < Columns; ++c) {
            for (size_t r = 0; r < Rows; ++r) {
                result.pushValue(c, r, operator()(c, r) || s);
            }
        }
        return result;
    }

    template<size_t Columns, size_t Rows, typename Type, typename Representation
        , typename Allocator>
    template<typename OAlloc>
    Vec<Columns, Type, Allocator>
    Mat<Columns, Rows, Type, Representation, Allocator>::operator*(
        const Vec<Columns, Type, OAlloc>& other) const requires
        (HasAdd<Type> && HasMul<Type>) {
        auto transposed = transpose();
        return Vec<Columns, Type, Allocator>([&](size_t r) {
            return transposed.column(r).dot(other);
        });
    }

    template<size_t Columns, size_t Rows, typename Type, typename Representation
        , typename Allocator>
    template<typename ORep, typename OAlloc>
    Mat<Columns, Rows, Type, Representation, Allocator>
    Mat<Columns, Rows, Type, Representation, Allocator>::operator+(
        const Mat<Columns, Rows, Type, ORep, OAlloc>& other) const requires
        HasAdd<Type> {
        Self result;

        for (size_t c = 0; c < Columns; ++c) {
            for (size_t r = 0; r < Rows; ++r) {
                result.pushValue(c, r, operator()(c, r) + other(c, r));
            }
        }

        return result;
    }

    template<size_t Columns, size_t Rows, typename Type, typename Representation
        , typename Allocator>
    template<typename ORep, typename OAlloc>
    Mat<Columns, Rows, Type, Representation, Allocator>
    Mat<Columns, Rows, Type, Representation, Allocator>::operator-(
        const Mat<Columns, Rows, Type, ORep, OAlloc>& other) const requires
        HasSub<Type> {
        Self result;

        for (size_t c = 0; c < Columns; ++c) {
            for (size_t r = 0; r < Rows; ++r) {
                result.pushValue(c, r, operator()(c, r) - other(c, r));
            }
        }

        return result;
    }

    template<size_t Columns, size_t Rows, typename Type,
        typename Representation, typename Allocator>
    template<size_t OC, size_t OR, typename ORep, typename OAlloc>
    Mat<OC, Rows, Type, Representation, Allocator>
    Mat<Columns, Rows, Type, Representation, Allocator>::operator*(
        const Mat<OC, OR, Type, ORep, OAlloc>& other) const requires
        (Columns == OR && HasAdd<Type> && HasMul<Type>) {
        auto transposed = transpose();
        return Mat<OC, Rows, Type, Representation, Allocator>(
            [&](size_t c, size_t r) {
                return transposed.column(r).dot(other.column(c));
            });
    }

    template<size_t Columns, size_t Rows, typename Type, typename Representation, typename Allocator>
    template<typename ORep, typename OAlloc>
    bool Mat<Columns, Rows, Type, Representation, Allocator>::operator==(
        const Mat<Columns, Rows, Type, ORep, OAlloc>& other) const {
        if constexpr (std::is_same_v<Mat, Mat<Columns, Rows, Type, OAlloc>>) {
            if (this == &other) return true;
        }
        return std::equal(sparseBegin(), sparseEnd(), other.sparseBegin(), other.sparseEnd());
    }

    template<size_t Columns, size_t Rows, typename Type, typename Representation, typename Allocator>
    template<typename ORep, typename OAlloc>
    bool Mat<Columns, Rows, Type, Representation, Allocator>::operator!=(
        const Mat<Columns, Rows, Type, ORep, OAlloc>& other) const {
        if constexpr (std::is_same_v<Mat, Mat<Columns, Rows, Type, OAlloc>>) {
            if (this == &other) return false;
        }
        return !std::equal(sparseBegin(), sparseEnd(), other.sparseBegin(), other.sparseEnd());
    }

    template<size_t Columns, size_t Rows, typename Type, typename Representation
        , typename Allocator>
    auto Mat<Columns, Rows, Type, Representation, Allocator>::begin() {
        return rep.begin();
    }

    template<size_t Columns, size_t Rows, typename Type, typename Representation
        , typename Allocator>
    auto Mat<Columns, Rows, Type, Representation, Allocator>::end() {
        return rep.end();
    }

    template<size_t Columns, size_t Rows, typename Type, typename Representation
        , typename Allocator>
    auto Mat<Columns, Rows, Type, Representation, Allocator>::cbegin() const {
        return rep.cbegin();
    }

    template<size_t Columns, size_t Rows, typename Type, typename Representation
        , typename Allocator>
    auto Mat<Columns, Rows, Type, Representation, Allocator>::cend() const {
        return rep.cend();
    }

    template<size_t Columns, size_t Rows, typename Type, typename Representation
        , typename Allocator>
    auto Mat<Columns, Rows, Type, Representation, Allocator>::rbegin() {
        return rep.rbegin();
    }

    template<size_t Columns, size_t Rows, typename Type, typename Representation
        , typename Allocator>
    auto Mat<Columns, Rows, Type, Representation, Allocator>::rend() {
        return rep.rend();
    }

    template<size_t Columns, size_t Rows, typename Type, typename Representation
        , typename Allocator>
    auto Mat<Columns, Rows, Type, Representation, Allocator>::crbegin() const {
        return rep.crbegin();
    }

    template<size_t Columns, size_t Rows, typename Type, typename Representation
        , typename Allocator>
    auto Mat<Columns, Rows, Type, Representation, Allocator>::crend() const {
        return rep.crend();
    }

    template<size_t Columns, size_t Rows, typename Type, typename Representation, typename Allocator>
    auto Mat<Columns, Rows, Type, Representation, Allocator>::sparseBegin() const {
        return rep.sparseBegin();
    }

    template<size_t Columns, size_t Rows, typename Type, typename Representation, typename Allocator>
    auto Mat<Columns, Rows, Type, Representation, Allocator>::sparseEnd() const {
        return rep.sparseEnd();
    }

    template<size_t Columns, size_t Rows, typename Type, typename Representation, typename Allocator>
    auto Mat<Columns, Rows, Type, Representation, Allocator>::reverseSparseBegin() const {
        return rep.reverseSparseBegin();
    }

    template<size_t Columns, size_t Rows, typename Type, typename Representation, typename Allocator>
    auto Mat<Columns, Rows, Type, Representation, Allocator>::reverseSparseEnd() const {
        return rep.reverseSparseEnd();
    }

    template<size_t Columns, size_t Rows, typename Type, typename Representation
        , typename Allocator>
    Mat<Columns, Rows, Type, Representation, Allocator>
    Mat<Columns, Rows, Type, Representation, Allocator>::translate(
        const Vec<3, Type>& t) requires (Columns == 4 && Rows == 4) {
        Type o = Type(1);
        Type z = Type(0);
        return Mat(
            o, z, z, z,
            z, o, z, z,
            z, z, o, z,
            t[0], t[1], t[2], o
        );
    }

    template<size_t Columns, size_t Rows, typename Type, typename Representation
        , typename Allocator>
    Mat<Columns, Rows, Type, Representation, Allocator>
    Mat<Columns, Rows, Type, Representation, Allocator>::scale(
        const rush::Vec<3, Type>& s) requires (Columns == 4 && Rows == 4) {
        Type o = Type(1);
        Type z = Type(0);
        return Mat(
            s[0], z, z, z,
            z, s[1], z, z,
            z, z, s[2], z,
            z, z, z, o
        );
    }

    template<size_t Columns, size_t Rows, typename Type, typename Representation
        , typename Allocator>
    Mat<Columns, Rows, Type, Representation, Allocator>
    Mat<Columns, Rows, Type, Representation, Allocator>::rotationX(Type radians)
        requires (
            Columns == 4 && Rows == 4) {
        Type c = std::cos(radians);
        Type s = std::sin(radians);
        Type o = Type(1);
        Type z = Type(0);
        return Mat(
            o, z, z, z,
            z, c, s, z,
            z, -s, c, z,
            z, z, z, o
        );
    }

    template<size_t Columns, size_t Rows, typename Type, typename Representation
        , typename Allocator>
    Mat<Columns, Rows, Type, Representation, Allocator>
    Mat<Columns, Rows, Type, Representation, Allocator>::rotationY(Type radians)
        requires (
            Columns == 4 && Rows == 4) {
        Type c = std::cos(radians);
        Type s = std::sin(radians);
        Type o = Type(1);
        Type z = Type(0);
        return Mat(
            c, z, -s, z,
            z, o, z, z,
            s, z, c, z,
            z, z, z, o
        );
    }

    template<size_t Columns, size_t Rows, typename Type, typename Representation
        , typename Allocator>
    Mat<Columns, Rows, Type, Representation, Allocator>
    Mat<Columns, Rows, Type, Representation, Allocator>::rotationZ(Type radians)
        requires (
            Columns == 4 && Rows == 4) {
        Type c = std::cos(radians);
        Type s = std::sin(radians);
        Type o = Type(1);
        Type z = Type(0);
        return Mat(
            c, s, z, z,
            -s, c, z, z,
            z, z, o, z,
            z, z, z, o
        );
    }

    template<size_t Columns, size_t Rows, typename Type, typename Representation
        , typename Allocator>
    Mat<Columns, Rows, Type, Representation, Allocator>
    Mat<Columns, Rows, Type, Representation, Allocator>::model(
        const Vec<3, Type>& s,
        const Quat<Type>& r,
        const Vec<3, Type>& t) requires (
        Columns == 4 && Rows == 4) {
        Mat result;
        Type* p = result.toPointer();
        *p = (Type(1) - Type(2) * (r.y * r.y + r.z * r.z)) * s.x();
        *++p = (r.x * r.y + r.z * r.s) * s.x() * Type(2);
        *++p = (r.x * r.z - r.y * r.s) * s.x() * Type(2);
        *++p = Type(0);
        *++p = (r.x * r.y - r.z * r.s) * s.y() * Type(2);
        *++p = (Type(1) - Type(2) * (r.x * r.x + r.z * r.z)) * s.y();
        *++p = (r.y * r.z + r.x * r.s) * s.y() * Type(2);
        *++p = Type(0);
        *++p = (r.x * r.z + r.y * r.s) * s.z() * Type(2);
        *++p = (r.y * r.z - r.x * r.s) * s.z() * Type(2);
        *++p = (Type(1) - Type(2) * (r.x * r.x + r.y * r.y)) * s.z();
        *++p = Type(0);
        *++p = t.x();
        *++p = t.y();
        *++p = t.z();
        *++p = Type(1);
        return result;
    }

    template<size_t Columns, size_t Rows, typename Type, typename Representation
        , typename Allocator>
    Mat<Columns, Rows, Type, Representation, Allocator>
    Mat<Columns, Rows, Type, Representation, Allocator>::normal(
        const Vec<3, Type>& s,
        const Quat<Type>& r) requires (
        Columns == 4 && Rows == 4) {
        Vec<3, Type> si = Type(1) / s;

        Mat result;
        Type* p = result.toPointer();
        *p = (Type(1) - Type(2) * (r.y * r.y + r.z * r.z)) * si.x();
        *++p = (r.x * r.y + r.z * r.s) * si.x() * Type(2);
        *++p = (r.x * r.z - r.y * r.s) * si.x() * Type(2);
        *++p = Type(0);
        *++p = (r.x * r.y - r.z * r.s) * si.y() * Type(2);
        *++p = (Type(1) - Type(2) * (r.x * r.x + r.z * r.z)) * si.y();
        *++p = (r.y * r.z + r.x * r.s) * si.y() * Type(2);
        *++p = Type(0);
        *++p = (r.x * r.z + r.y * r.s) * si.z() * Type(2);
        *++p = (r.y * r.z - r.x * r.s) * si.z() * Type(2);
        *++p = (Type(1) - Type(2) * (r.x * r.x + r.y * r.y)) * si.z();
        *++p = Type(0);
        *++p = Type(0);
        *++p = Type(0);
        *++p = Type(0);
        *++p = Type(1);
        return result;
    }


    template<size_t Columns, size_t Rows, typename Type, typename Representation
        , typename Allocator>
    template<Hand H>
    Mat<Columns, Rows, Type, Representation, Allocator>
    Mat<Columns, Rows, Type, Representation, Allocator>::lookAt(
        const Vec<3, Type>& origin,
        const Vec<3, Type>& direction,
        const Vec<3, Type>& up) requires (Columns == 4 && Rows == 4) {
        Vec<3, Type> f = -direction.normalized();
        Vec<3, Type> u = (up - up.dot(f) / f.squaredLength() * f).normalized();
        Vec<3, Type> s = u.cross(f);

        if constexpr (H == Hand::Left) {
            s = -s;
        }

        Mat m(Type(1));
        m(0, 0) = s[0];
        m(1, 0) = s[1];
        m(2, 0) = s[2];
        m(0, 1) = u[0];
        m(1, 1) = u[1];
        m(2, 1) = u[2];
        m(0, 2) = f[0];
        m(1, 2) = f[1];
        m(2, 2) = f[2];
        m(3, 0) = -s.dot(origin);
        m(3, 1) = -u.dot(origin);
        m(3, 2) = -f.dot(origin);

        return m;
    }

    template<size_t Columns, size_t Rows, typename Type, typename Representation
        , typename Allocator>
    template<Hand Hand, ProjectionFormat Format>
    Mat<Columns, Rows, Type, Representation, Allocator>
    Mat<Columns, Rows, Type, Representation, Allocator>::frustum(
        Type left, Type right,
        Type bottom, Type top,
        Type n, Type f) requires (Columns == 4 && Rows == 4) {
        Mat m = Mat();
        m(0, 0) = Type(2) * n / (right - left);
        m(1, 1) = Type(2) * n / (top - bottom);

        if constexpr (Format == ProjectionFormat::DirectX) {
            if constexpr (Hand == Hand::Left) {
                m(2, 0) = -(right + left) / (right - left);
                m(2, 1) = -(top + bottom) / (top - bottom);
                m(2, 2) = f / (f - n);
                m(2, 3) = Type(1);
                m(3, 2) = -f * n / (f - n);
            } else {
                m(2, 0) = (right + left) / (right - left);
                m(2, 1) = (top + bottom) / (top - bottom);
                m(2, 2) = f / (f - n);
                m(2, 3) = -Type(1);
                m(3, 2) = -f * n / (f - n);
            }
        } else {
            if constexpr (Hand == Hand::Left) {
                m(2, 0) = -(right + left) / (right - left);
                m(2, 1) = -(top + bottom) / (top - bottom);
                m(2, 2) = (f + n) / (f - n);
                m(2, 3) = Type(1);
                m(3, 2) = -Type(2) * f * n / (f - n);
            } else {
                m(2, 0) = (right + left) / (right - left);
                m(2, 1) = (top + bottom) / (top - bottom);
                m(2, 2) = -(f + n) / (f - n);
                m(2, 3) = -Type(1);
                m(3, 2) = -Type(2) * f * n / (f - n);
            }
        }

        if constexpr (Format == ProjectionFormat::Vulkan) {
            m(1, 1) *= -1;
        }

        return m;
    }

    template<size_t Columns, size_t Rows, typename Type, typename Representation
        , typename Allocator>
    template<Hand Hand, ProjectionFormat Format>
    Mat<Columns, Rows, Type, Representation, Allocator>
    Mat<Columns, Rows, Type, Representation, Allocator>::orthogonal(
        Type left, Type right,
        Type bottom, Type top,
        Type n, Type f) requires (Columns == 4 && Rows == 4) {
        Mat m = Mat(Type(1));
        m(0, 0) = Type(2) / (right - left);
        m(1, 1) = Type(2) / (top - bottom);

        if constexpr (Format == ProjectionFormat::DirectX) {
            if constexpr (Hand == Hand::Left) {
                m(2, 2) = Type(1) / (f - n);
                m(3, 0) = -(right + left) / (right - left);
                m(3, 1) = -(top + bottom) / (top - bottom);
                m(3, 2) = -n / (f - n);
            } else {
                m(2, 2) = -Type(1) / (f - n);
                m(3, 0) = -(right + left) / (right - left);
                m(3, 1) = -(top + bottom) / (top - bottom);
                m(3, 2) = -n / (f - n);
            }
        } else {
            if constexpr (Hand == Hand::Left) {
                m(2, 2) = Type(2) / (f - n);
                m(3, 0) = -(right + left) / (right - left);
                m(3, 1) = -(top + bottom) / (top - bottom);
                m(3, 2) = -(f + n) / (f - n);
            } else {
                m(2, 2) = -Type(2) / (f - n);
                m(3, 0) = -(right + left) / (right - left);
                m(3, 1) = -(top + bottom) / (top - bottom);
                m(3, 2) = -(f + n) / (f - n);
            }
        }

        if constexpr (Format == ProjectionFormat::Vulkan) {
            m(1, 1) *= -1;
        }

        return m;
    }

    template<size_t Columns, size_t Rows, typename Type, typename Representation
        , typename Allocator>
    template<Hand Hand, ProjectionFormat Format>
    Mat<Columns, Rows, Type, Representation, Allocator>
    Mat<Columns, Rows, Type, Representation, Allocator>::perspective(Type fovY,
                                                                     Type aspectRatio,
                                                                     Type n,
                                                                     Type f) requires (
        Columns == 4 && Rows == 4) {
        Type top = std::tan(fovY / Type(2)) * n;
        Type right = top * aspectRatio;
        return frustum<Hand, Format>(-right, right, -top, top, n, f);
    }

    template<size_t Columns, size_t Rows, typename Type, typename Representation
        , typename Allocator>
    template<Hand Hand>
    Mat<Columns, Rows, Type, Representation, Allocator>
    Mat<Columns, Rows, Type, Representation, Allocator>::infinitePerspective(
        Type fovY, Type aspectRatio, Type n) requires (
        Columns == 4 && Rows == 4) {
        Type top = std::tan(fovY / Type(2)) * n;
        Type right = top * aspectRatio;

        Mat m = Mat();
        m(0, 0) = n / right;
        m(1, 1) = n / top;

        m(3, 2) = -Type(2) * n;

        if constexpr (Hand == Hand::Left) {
            m(2, 2) = Type(1);
            m(2, 3) = Type(1);
        } else {
            m(2, 2) = -Type(1);
            m(2, 3) = -Type(1);
        }

        return m;
    }
}

#endif //RUSH_MAT_IMPL_H
