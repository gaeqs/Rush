//
// Created by gaelr on 12/08/2023.
//

#ifndef RUSH_MAT_IMPL_H
#define RUSH_MAT_IMPL_H

#include <rush/quaternion/quat.h>

namespace rush {

    template<size_t Columns, size_t Rows, typename Type, typename Allocator>
    template<typename... T>
    requires (std::is_convertible_v<std::common_type_t<T...>, Type>
              && sizeof...(T) <= Columns * Rows)
    Mat<Columns, Rows, Type, Allocator>::Mat(T... list) {
        Type* ptr = toPointer();
        ((*ptr++ = list), ...);
    }

    template<size_t Columns, size_t Rows, typename Type, typename Allocator>
    Mat<Columns, Rows, Type, Allocator>::Mat() : data() {

    }

    template<size_t Columns, size_t Rows, typename Type, typename Allocator>
    Mat<Columns, Rows, Type, Allocator>::Mat(Type diagonal) : data() {
        for (size_t i = 0; i < std::min(Columns, Rows); ++i) {
            data[i][i] = diagonal;
        }
    }

    template<size_t Columns, size_t Rows, typename Type, typename Allocator>
    Mat<Columns, Rows, Type, Allocator>::Mat(
            std::function<Type(size_t, size_t)> populator) {
        for (size_t c = 0; c < Columns; ++c) {
            for (size_t r = 0; r < Rows; ++r) {
                data[c][r] = populator(c, r);
            }
        }
    }

    template<size_t Columns, size_t Rows, typename Type, typename Allocator>
    Mat<Columns, Rows, Type, Allocator>::Mat(
            std::function<Type(size_t, size_t, size_t, size_t)> populator) {
        for (size_t c = 0; c < Columns; ++c) {
            for (size_t r = 0; r < Rows; ++r) {
                data[c][r] = populator(c, r, Columns, Rows);
            }
        }
    }

    template<size_t Columns, size_t Rows, typename Type, typename Allocator>
    template<size_t OColumns, size_t ORows, typename OAlloc>
    requires(Columns > OColumns || Rows > ORows)
    Mat<Columns, Rows, Type, Allocator>::Mat(
            const Mat<OColumns, ORows, Type, OAlloc>& other, Type diagonal) :
            Mat<Columns, Rows, Type, Allocator>(diagonal) {
        for (size_t c = 0; c < std::min(Columns, OColumns); ++c) {
            for (size_t r = 0; r < std::min(Rows, ORows); ++r) {
                data[c][r] = other.data[c][r];
            }
        }
    }

    template<size_t Columns, size_t Rows, typename Type, typename Allocator>
    constexpr size_t Mat<Columns, Rows, Type, Allocator>::size() const {
        return Columns * Rows;
    }

    template<size_t Columns, size_t Rows, typename Type, typename Allocator>
    const Type* Mat<Columns, Rows, Type, Allocator>::toPointer() const {
        return data[0].toPointer();
    }

    template<size_t Columns, size_t Rows, typename Type, typename Allocator>
    Type* Mat<Columns, Rows, Type, Allocator>::toPointer() {
        return data[0].toPointer();
    }

    template<size_t Columns, size_t Rows, typename Type, typename Allocator>
    const typename Mat<Columns, Rows, Type, Allocator>::ColumnType&
    Mat<Columns, Rows, Type, Allocator>::column(size_t column) const {
        return (*this)[column];
    }

    template<size_t Columns, size_t Rows, typename Type, typename Allocator>
    typename Mat<Columns, Rows, Type, Allocator>::ColumnType&
    Mat<Columns, Rows, Type, Allocator>::column(size_t column) {
        return (*this)[column];
    }

    template<size_t Columns, size_t Rows, typename Type, typename Allocator>
    Vec<Columns, Type, StaticAllocator>
    Mat<Columns, Rows, Type, Allocator>::row(size_t row) const {
        Vec<Columns, Type, StaticAllocator> vec;
        for (size_t i = 0; i < Columns; ++i) {
            vec[i] = data[i][row];
        }
        return vec;
    }

    template<size_t Columns, size_t Rows, typename Type, typename Allocator>
    VecRef<Columns, Type> Mat<Columns, Rows, Type, Allocator>::row(size_t row) {
        VecRef<Columns, Type> vec;
        for (size_t i = 0; i < Columns; ++i) {
            vec.references[i] = &data[i][row];
        }
        return vec;
    }

    template<size_t Columns, size_t Rows, typename Type, typename Allocator>
    const typename Mat<Columns, Rows, Type, Allocator>::ColumnType&
    Mat<Columns, Rows, Type, Allocator>::operator[](size_t column) const {
        return data[column];
    }

    template<size_t Columns, size_t Rows, typename Type, typename Allocator>
    typename Mat<Columns, Rows, Type, Allocator>::ColumnType&
    Mat<Columns, Rows, Type, Allocator>::operator[](size_t column) {
        return data[column];
    }

    template<size_t Columns, size_t Rows, typename Type, typename Allocator>
    Type&
    Mat<Columns, Rows, Type, Allocator>::operator()(size_t column, size_t row) {
        return data[column][row];
    }

    template<size_t Columns, size_t Rows, typename Type, typename Allocator>
    const Type&
    Mat<Columns, Rows, Type, Allocator>::operator()(size_t column,
                                                    size_t row) const {
        return data[column][row];
    }

    template<size_t Columns, size_t Rows, typename Type, typename Allocator>
    Type Mat<Columns, Rows, Type, Allocator>::determinant() const requires
    HasAdd<Type> &&
    HasSub<Type> &&
    HasMul<Type> &&
    HasDiv<Type> &&
    (Columns ==
     Rows) {
        if constexpr (Columns == 1) {
            return data[0][0];
        } else if constexpr (Columns == 2) {
            return data[0][0] * data[1][1] - data[0][1] * data[1][0];
        } else if constexpr (Columns == 3) {
            return +data[0][0] *
                   (data[1][1] * data[2][2] - data[1][2] * data[2][1])
                   - data[0][1] *
                     (data[1][0] * data[2][2] - data[1][2] * data[2][0])
                   + data[0][2] *
                     (data[1][0] * data[2][1] - data[1][1] * data[2][0]);
        } else {
            // Generic method
            Self tempM = *this;
            auto& temp = tempM.data;

            Type det = temp[0][0];
            for (size_t i = 0; i < Columns; i++) {
                size_t pivot = i;
                for (size_t j = i + 1; j < Columns; j++) {
                    if (std::abs(temp[j][i]) > std::abs(temp[pivot][i])) {
                        pivot = j;
                    }
                }

                if (pivot != i) {
                    std::swap(temp[i], temp[pivot]);
                    det = -det;
                }

                if (i != 0) {
                    det *= temp[i][i];
                }
                if (temp[i][i] == 0) return det;

                for (size_t j = i + 1; j < Columns; j++) {
                    Type factor = temp[j][i] / temp[i][i];
                    for (size_t k = i + 1; k < Columns; k++) {
                        temp[j][k] -= factor * temp[i][k];
                    }
                }
            }
            return det;
        }
    }

    template<size_t Columns, size_t Rows, typename Type, typename Allocator>
    Mat<Rows, Columns, Type, Allocator>
    Mat<Columns, Rows, Type, Allocator>::transpose() const {
        return Mat<Rows, Columns, Type, Allocator>
                ([this](size_t c, size_t r) {
                    return operator()(r, c);
                });
    }

    template<size_t Columns, size_t Rows, typename Type, typename Allocator>
    Mat<Columns, Rows, Type, Allocator>::Self
    Mat<Columns, Rows, Type, Allocator>::inverse() const requires
    HasAdd<Type> &&
    HasSub<Type> &&
    HasMul<Type> &&
    HasDiv<Type> &&
    (Columns ==
     Rows) &&
    (Columns < 5) {
        auto& d = data;
        if constexpr (Columns == 1) {
            return {Type(1) / d[0][0]};
        } else if constexpr (Columns == 2) {
            Type det = determinant();
            return {d[1][1] / det, -d[0][1] / det,
                    -d[1][0] / det, d[0][0] / det};
        } else if constexpr (Columns == 3) {
            Type det = determinant();
            Self inverse;
            inverse[0][0] = +(d[1][1] * d[2][2] - d[2][1] * d[1][2]);
            inverse[1][0] = -(d[1][0] * d[2][2] - d[2][0] * d[1][2]);
            inverse[2][0] = +(d[1][0] * d[2][1] - d[2][0] * d[1][1]);
            inverse[0][1] = -(d[0][1] * d[2][2] - d[2][1] * d[0][2]);
            inverse[1][1] = +(d[0][0] * d[2][2] - d[2][0] * d[0][2]);
            inverse[2][1] = -(d[0][0] * d[2][1] - d[2][0] * d[0][1]);
            inverse[0][2] = +(d[0][1] * d[1][2] - d[1][1] * d[0][2]);
            inverse[1][2] = -(d[0][0] * d[1][2] - d[1][0] * d[0][2]);
            inverse[2][2] = +(d[0][0] * d[1][1] - d[1][0] * d[0][1]);
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
            inv[0][0] = +(d[1][1] * s00 - d[1][2] * s01 + d[1][3] * s02);
            inv[1][0] = -(d[1][0] * s00 - d[1][2] * s03 + d[1][3] * s04);
            inv[2][0] = +(d[1][0] * s01 - d[1][1] * s03 + d[1][3] * s05);
            inv[3][0] = -(d[1][0] * s02 - d[1][1] * s04 + d[1][2] * s05);
            inv[0][1] = -(d[0][1] * s00 - d[0][2] * s01 + d[0][3] * s02);
            inv[1][1] = +(d[0][0] * s00 - d[0][2] * s03 + d[0][3] * s04);
            inv[2][1] = -(d[0][0] * s01 - d[0][1] * s03 + d[0][3] * s05);
            inv[3][1] = +(d[0][0] * s02 - d[0][1] * s04 + d[0][2] * s05);
            inv[0][2] = +(d[0][1] * s06 - d[0][2] * s07 + d[0][3] * s08);
            inv[1][2] = -(d[0][0] * s06 - d[0][2] * s09 + d[0][3] * s10);
            inv[2][2] = +(d[0][0] * s07 - d[0][1] * s09 + d[0][3] * s11);
            inv[3][2] = -(d[0][0] * s08 - d[0][1] * s10 + d[0][2] * s11);
            inv[0][3] = -(d[0][1] * s12 - d[0][2] * s13 + d[0][3] * s14);
            inv[1][3] = +(d[0][0] * s12 - d[0][2] * s15 + d[0][3] * s16);
            inv[2][3] = -(d[0][0] * s13 - d[0][1] * s15 + d[0][3] * s17);
            inv[3][3] = +(d[0][0] * s14 - d[0][1] * s16 + d[0][2] * s17);

            Type det = d[0][0] * inv[0][0] + d[0][1] * inv[1][0]
                       + d[0][2] * inv[2][0] + d[0][3] * inv[3][0];

            return inv / det;
        }
    }

    template<size_t Columns, size_t Rows, typename Type, typename Allocator>
    typename Mat<Columns, Rows, Type, Allocator>::Self&
    Mat<Columns, Rows, Type, Allocator>::operator+() {
        return *this;
    }

    template<size_t Columns, size_t Rows, typename Type, typename Allocator>
    typename Mat<Columns, Rows, Type, Allocator>::Self&
    Mat<Columns, Rows, Type, Allocator>::operator+() const {
        return *this;
    }

    template<size_t Columns, size_t Rows, typename Type, typename Allocator>
    Mat<Columns, Rows, Type, Allocator>::Self
    Mat<Columns, Rows, Type, Allocator>::operator-() const requires HasSub<Type> {
        Self result;
        for (size_t i = 0; i < Columns; ++i) {
            result[i] = -data[i];
        }
        return result;
    }

    template<size_t Columns, size_t Rows, typename Type, typename Allocator>
    typename Mat<Columns, Rows, Type, Allocator>::Self&
    Mat<Columns, Rows, Type, Allocator>::operator+=(
            const Type& s) requires HasAdd<Type> {
        for (size_t i = 0; i < Columns; ++i) {
            data[i] += s;
        }
        return *this;
    }

    template<size_t Columns, size_t Rows, typename Type, typename Allocator>
    typename Mat<Columns, Rows, Type, Allocator>::Self&
    Mat<Columns, Rows, Type, Allocator>::operator-=(
            const Type& s) requires HasSub<Type> {
        for (size_t i = 0; i < Columns; ++i) {
            data[i] -= s;
        }
        return *this;
    }

    template<size_t Columns, size_t Rows, typename Type, typename Allocator>
    typename Mat<Columns, Rows, Type, Allocator>::Self&
    Mat<Columns, Rows, Type, Allocator>::operator*=(
            const Type& s) requires HasMul<Type> {
        for (size_t i = 0; i < Columns; ++i) {
            data[i] *= s;
        }
        return *this;
    }

    template<size_t Columns, size_t Rows, typename Type, typename Allocator>
    typename Mat<Columns, Rows, Type, Allocator>::Self&
    Mat<Columns, Rows, Type, Allocator>::operator/=(
            const Type& s) requires HasDiv<Type> {
        for (size_t i = 0; i < Columns; ++i) {
            data[i] /= s;
        }
        return *this;
    }

    template<size_t Columns, size_t Rows, typename Type, typename Allocator>
    typename Mat<Columns, Rows, Type, Allocator>::Self&
    Mat<Columns, Rows, Type, Allocator>::operator<<=(
            const Type& s) requires HasShl<Type> {
        for (size_t i = 0; i < Columns; ++i) {
            data[i] <<= s;
        }
        return *this;
    }

    template<size_t Columns, size_t Rows, typename Type, typename Allocator>
    typename Mat<Columns, Rows, Type, Allocator>::Self&
    Mat<Columns, Rows, Type, Allocator>::operator>>=(
            const Type& s) requires HasShr<Type> {
        for (size_t i = 0; i < Columns; ++i) {
            data[i] >>= s;
        }
        return *this;
    }

    template<size_t Columns, size_t Rows, typename Type, typename Allocator>
    typename Mat<Columns, Rows, Type, Allocator>::Self&
    Mat<Columns, Rows, Type, Allocator>::operator&=(
            const Type& s) requires HasBitAnd<Type> {
        for (size_t i = 0; i < Columns; ++i) {
            data[i] &= s;
        }
        return *this;
    }

    template<size_t Columns, size_t Rows, typename Type, typename Allocator>
    typename Mat<Columns, Rows, Type, Allocator>::Self&
    Mat<Columns, Rows, Type, Allocator>::operator|=(
            const Type& s) requires HasBitOr<Type> {
        for (size_t i = 0; i < Columns; ++i) {
            data[i] |= s;
        }
        return *this;
    }

    template<size_t Columns, size_t Rows, typename Type, typename Allocator>
    typename Mat<Columns, Rows, Type, Allocator>::Self&
    Mat<Columns, Rows, Type, Allocator>::operator^=(
            const Type& s) requires HasBitXor<Type> {
        for (size_t i = 0; i < Columns; ++i) {
            data[i] ^= s;
        }
        return *this;
    }

    template<size_t Columns, size_t Rows, typename Type, typename Allocator>
    template<typename OAlloc>
    Mat<Columns, Rows, Type, Allocator>&
    Mat<Columns, Rows, Type, Allocator>::operator+=(
            const Mat<Columns, Rows, Type, OAlloc>& o) requires HasAdd<Type> {
        for (size_t i = 0; i < Columns; ++i) {
            data[i] += o.data[i];
        }
        return *this;
    }

    template<size_t Columns, size_t Rows, typename Type, typename Allocator>
    template<typename OAlloc>
    Mat<Columns, Rows, Type, Allocator>&
    Mat<Columns, Rows, Type, Allocator>::operator-=(
            const Mat<Columns, Rows, Type, OAlloc>& o) requires HasSub<Type> {
        for (size_t i = 0; i < Columns; ++i) {
            data[i] -= o.data[i];
        }
        return *this;
    }

    template<size_t Columns, size_t Rows, typename Type, typename Allocator>
    Mat<Columns, Rows, Type, Allocator>::Self
    Mat<Columns, Rows, Type, Allocator>::operator+(const Type& s) const requires
    HasAdd<Type> {
        Self result;
        for (size_t i = 0; i < Columns; ++i) {
            result[i] = data[i] + s;
        }
        return result;
    }

    template<size_t Columns, size_t Rows, typename Type, typename Allocator>
    Mat<Columns, Rows, Type, Allocator>::Self
    Mat<Columns, Rows, Type, Allocator>::operator-(const Type& s) const requires
    HasSub<Type> {
        Self result;
        for (size_t i = 0; i < Columns; ++i) {
            result[i] = data[i] - s;
        }
        return result;
    }

    template<size_t Columns, size_t Rows, typename Type, typename Allocator>
    Mat<Columns, Rows, Type, Allocator>::Self
    Mat<Columns, Rows, Type, Allocator>::operator*(const Type& s) const requires
    HasMul<Type> {
        Self result;
        for (size_t i = 0; i < Columns; ++i) {
            result[i] = data[i] * s;
        }
        return result;
    }

    template<size_t Columns, size_t Rows, typename Type, typename Allocator>
    Mat<Columns, Rows, Type, Allocator>::Self
    Mat<Columns, Rows, Type, Allocator>::operator/(const Type& s) const requires
    HasDiv<Type> {
        Self result;
        for (size_t i = 0; i < Columns; ++i) {
            result[i] = data[i] / s;
        }
        return result;
    }

    template<size_t Columns, size_t Rows, typename Type, typename Allocator>
    Mat<Columns, Rows, Type, Allocator>::Self
    Mat<Columns, Rows, Type, Allocator>::operator<<(
            const Type& s) const requires
    HasShl<Type> {
        Self result;
        for (size_t i = 0; i < Columns; ++i) {
            result[i] = data[i] << s;
        }
        return result;
    }

    template<size_t Columns, size_t Rows, typename Type, typename Allocator>
    Mat<Columns, Rows, Type, Allocator>::Self
    Mat<Columns, Rows, Type, Allocator>::operator>>(
            const Type& s) const requires
    HasShr<Type> {
        Self result;
        for (size_t i = 0; i < Columns; ++i) {
            result[i] = data[i] >> s;
        }
        return result;
    }

    template<size_t Columns, size_t Rows, typename Type, typename Allocator>
    Mat<Columns, Rows, Type, Allocator>::Self
    Mat<Columns, Rows, Type, Allocator>::operator&(const Type& s) const requires
    HasBitAnd<Type> {
        Self result;
        for (size_t i = 0; i < Columns; ++i) {
            result[i] = data[i] & s;
        }
        return result;
    }

    template<size_t Columns, size_t Rows, typename Type, typename Allocator>
    Mat<Columns, Rows, Type, Allocator>::Self
    Mat<Columns, Rows, Type, Allocator>::operator|(const Type& s) const requires
    HasBitOr<Type> {
        Self result;
        for (size_t i = 0; i < Columns; ++i) {
            result[i] = data[i] | s;
        }
        return result;
    }

    template<size_t Columns, size_t Rows, typename Type, typename Allocator>
    Mat<Columns, Rows, Type, Allocator>::Self
    Mat<Columns, Rows, Type, Allocator>::operator^(const Type& s) const requires
    HasBitXor<Type> {
        Self result;
        for (size_t i = 0; i < Columns; ++i) {
            result[i] = data[i] ^ s;
        }
        return result;
    }

    template<size_t Columns, size_t Rows, typename Type, typename Allocator>
    Mat<Columns, Rows, Type, Allocator>::Self
    Mat<Columns, Rows, Type, Allocator>::operator&&(
            const Type& s) const requires
    HasAnd<Type> {
        Self result;
        for (size_t i = 0; i < Columns; ++i) {
            result[i] = data[i] && s;
        }
        return result;
    }

    template<size_t Columns, size_t Rows, typename Type, typename Allocator>
    Mat<Columns, Rows, Type, Allocator>::Self
    Mat<Columns, Rows, Type, Allocator>::operator||(
            const Type& s) const requires
    HasOr<Type> {
        Self result;
        for (size_t i = 0; i < Columns; ++i) {
            result[i] = data[i] || s;
        }
        return result;
    }

    template<size_t Columns, size_t Rows, typename Type, typename Allocator>
    template<typename OAlloc>
    Vec<Columns, Type, Allocator>
    Mat<Columns, Rows, Type, Allocator>::operator*(
            const Vec<Columns, Type, OAlloc>& other) const requires
    (HasAdd<Type> && HasMul<Type>) {
        auto transposed = transpose();
        return Vec<Columns, Type, Allocator>([&](size_t r) {
            return transposed.column(r) % other;
        });

    }

    template<size_t Columns, size_t Rows, typename Type, typename Allocator>
    template<typename OAlloc>
    Mat<Columns, Rows, Type, Allocator>
    Mat<Columns, Rows, Type, Allocator>::operator+(
            const Mat<Columns, Rows, Type, OAlloc>& other) const requires
    HasAdd<Type> {
        Self result;
        for (size_t i = 0; i < Columns; ++i) {
            result[i] = data[i] + other[i];
        }
        return result;
    }

    template<size_t Columns, size_t Rows, typename Type, typename Allocator>
    template<typename OAlloc>
    Mat<Columns, Rows, Type, Allocator>
    Mat<Columns, Rows, Type, Allocator>::operator-(
            const Mat<Columns, Rows, Type, OAlloc>& other) const requires
    HasSub<Type> {
        Self result;
        for (size_t i = 0; i < Columns; ++i) {
            result[i] = data[i] - other[i];
        }
        return result;
    }

    template<size_t Columns, size_t Rows, typename Type, typename Allocator>
    template<size_t OC, size_t OR, typename OAlloc>
    Mat<OC, Rows, Type, Allocator>
    Mat<Columns, Rows, Type, Allocator>::operator*(
            const Mat<OC, OR, Type, OAlloc>& other) const requires
    (Columns == OR && HasAdd<Type> && HasMul<Type>) {
        auto transposed = transpose();
        return Mat<OC, Rows, Type, Allocator>([&](size_t c, size_t r) {
            return transposed.column(r) % other.column(c);
        });
    }

    template<size_t Columns, size_t Rows, typename Type, typename Allocator>
    template<typename OAlloc>
    bool Mat<Columns, Rows, Type, Allocator>::operator==(
            const Mat<Columns, Rows, Type, OAlloc>& other) const {
        if constexpr (std::is_same_v<Mat, Mat<Columns, Rows, Type, OAlloc>>) {
            if (this == &other) return true;
        }
        return std::equal(cbegin(), cend(), other.cbegin());
    }

    template<size_t Columns, size_t Rows, typename Type, typename Allocator>
    template<typename OAlloc>
    bool Mat<Columns, Rows, Type, Allocator>::operator!=(
            const Mat<Columns, Rows, Type, OAlloc>& other) const {
        if constexpr (std::is_same_v<Mat, Mat<Columns, Rows, Type, OAlloc>>) {
            if (this == &other) return false;
        }
        return !std::equal(cbegin(), cend(), other.cbegin());
    }

    template<size_t Columns, size_t Rows, typename Type, typename Allocator>
    auto Mat<Columns, Rows, Type, Allocator>::begin() {
        return data.begin();
    }

    template<size_t Columns, size_t Rows, typename Type, typename Allocator>
    auto Mat<Columns, Rows, Type, Allocator>::end() {
        return data.end();
    }

    template<size_t Columns, size_t Rows, typename Type, typename Allocator>
    auto Mat<Columns, Rows, Type, Allocator>::cbegin() const {
        return data.cbegin();
    }

    template<size_t Columns, size_t Rows, typename Type, typename Allocator>
    auto Mat<Columns, Rows, Type, Allocator>::cend() const {
        return data.cend();
    }

    template<size_t Columns, size_t Rows, typename Type, typename Allocator>
    auto Mat<Columns, Rows, Type, Allocator>::rbegin() {
        return data.rbegin();
    }

    template<size_t Columns, size_t Rows, typename Type, typename Allocator>
    auto Mat<Columns, Rows, Type, Allocator>::rend() {
        return data.rend();
    }

    template<size_t Columns, size_t Rows, typename Type, typename Allocator>
    auto Mat<Columns, Rows, Type, Allocator>::crbegin() const {
        return data.crbegin();
    }

    template<size_t Columns, size_t Rows, typename Type, typename Allocator>
    auto Mat<Columns, Rows, Type, Allocator>::crend() const {
        return data.crend();
    }

    template<size_t Columns, size_t Rows, typename Type, typename Allocator>
    Mat<Columns, Rows, Type, Allocator>
    Mat<Columns, Rows, Type, Allocator>::translate(
            const rush::Vec<3, Type>& t) requires (Columns == 4 && Rows == 4) {
        Type o = Type(1);
        Type z = Type(0);
        return Mat(
                o, z, z, z,
                z, o, z, z,
                z, z, o, z,
                t[0], t[1], t[2], o
        );
    }

    template<size_t Columns, size_t Rows, typename Type, typename Allocator>
    Mat<Columns, Rows, Type, Allocator>
    Mat<Columns, Rows, Type, Allocator>::scale(
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

    template<size_t Columns, size_t Rows, typename Type, typename Allocator>
    Mat<Columns, Rows, Type, Allocator>
    Mat<Columns, Rows, Type, Allocator>::rotationX(Type radians) requires (
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

    template<size_t Columns, size_t Rows, typename Type, typename Allocator>
    Mat<Columns, Rows, Type, Allocator>
    Mat<Columns, Rows, Type, Allocator>::rotationY(Type radians) requires (
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

    template<size_t Columns, size_t Rows, typename Type, typename Allocator>
    Mat<Columns, Rows, Type, Allocator>
    Mat<Columns, Rows, Type, Allocator>::rotationZ(Type radians) requires (
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

    template<size_t Columns, size_t Rows, typename Type, typename Allocator>
    Mat<Columns, Rows, Type, Allocator>
    Mat<Columns, Rows, Type, Allocator>::model(const Vec<3, Type>& s,
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

    template<size_t Columns, size_t Rows, typename Type, typename Allocator>
    Mat<Columns, Rows, Type, Allocator>
    Mat<Columns, Rows, Type, Allocator>::normal(const Vec<3, Type>& s,
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


    template<size_t Columns, size_t Rows, typename Type, typename Allocator>
    template<Hand H>
    Mat<Columns, Rows, Type, Allocator>
    Mat<Columns, Rows, Type, Allocator>::lookAt(
            const Vec<3, Type>& origin,
            const Vec<3, Type>& direction,
            const Vec<3, Type>& up) requires (Columns == 4 && Rows == 4) {
        Vec<3, Type> f = direction.normalized();
        Vec<3, Type> s = up.cross(f).normalized();
        Vec<3, Type> u = f.cross(s);

        Mat m(Type(1));
        m(0, 0) = s[0];
        m(1, 0) = s[1];
        m(2, 0) = s[2];
        m(0, 1) = u[0];
        m(1, 1) = u[1];
        m(2, 1) = u[2];

        if constexpr (H == Hand::Left) {
            m(0, 2) = f[0];
            m(1, 2) = f[1];
            m(2, 2) = f[2];
            m(3, 0) = -s.dot(origin);
            m(3, 1) = -u.dot(origin);
            m(3, 2) = -f.dot(origin);
        } else {
            m(0, 2) = -f[0];
            m(1, 2) = -f[1];
            m(2, 2) = -f[2];
            m(3, 0) = -s.dot(origin);
            m(3, 1) = -u.dot(origin);
            m(3, 2) = f.dot(origin);
        }
        return m;
    }

    template<size_t Columns, size_t Rows, typename Type, typename Allocator>
    template<Hand Hand, ProjectionFormat Format>
    Mat<Columns, Rows, Type, Allocator>
    Mat<Columns, Rows, Type, Allocator>::frustum(
            Type left, Type right,
            Type bottom, Type top,
            Type near, Type far) requires (Columns == 4 && Rows == 4) {
        Mat m = Mat();
        m(0, 0) = Type(2) * near / (right - left);
        m(1, 1) = Type(2) * near / (top - bottom);

        if constexpr (Format == ProjectionFormat::DirectX) {
            if constexpr (Hand == Hand::Left) {
                m(2, 0) = -(right + left) / (right - left);
                m(2, 1) = -(top + bottom) / (top - bottom);
                m(2, 2) = far / (far - near);
                m(2, 3) = Type(1);
                m(3, 2) = -far * near / (far - near);
            } else {
                m(2, 0) = (right + left) / (right - left);
                m(2, 1) = (top + bottom) / (top - bottom);
                m(2, 2) = far / (far - near);
                m(2, 3) = -Type(1);
                m(3, 2) = -far * near / (far - near);
            }
        } else {
            if constexpr (Hand == Hand::Left) {
                m(2, 0) = -(right + left) / (right - left);
                m(2, 1) = -(top + bottom) / (top - bottom);
                m(2, 2) = (far + near) / (far - near);
                m(2, 3) = Type(1);
                m(3, 2) = -Type(2) * far * near / (far - near);
            } else {
                m(2, 0) = (right + left) / (right - left);
                m(2, 1) = (top + bottom) / (top - bottom);
                m(2, 2) = -(far + near) / (far - near);
                m(2, 3) = -Type(1);
                m(3, 2) = -Type(2) * far * near / (far - near);
            }
        }

        if constexpr (Format == ProjectionFormat::Vulkan) {
            m(1, 1) *= -1;
        }

        return m;
    }

    template<size_t Columns, size_t Rows, typename Type, typename Allocator>
    template<Hand Hand, ProjectionFormat Format>
    Mat<Columns, Rows, Type, Allocator>
    Mat<Columns, Rows, Type, Allocator>::orthogonal(
            Type left, Type right,
            Type bottom, Type top,
            Type near, Type far) requires (Columns == 4 && Rows == 4) {

        Mat m = Mat(Type(1));
        m(0, 0) = Type(2) / (right - left);
        m(1, 1) = Type(2) / (top - bottom);

        if constexpr (Format == ProjectionFormat::DirectX) {
            if constexpr (Hand == Hand::Left) {
                m(2, 2) = Type(1) / (far - near);
                m(3, 0) = -(right + left) / (right - left);
                m(3, 1) = -(top + bottom) / (top - bottom);
                m(3, 2) = -near / (far - near);
            } else {
                m(2, 2) = -Type(1) / (far - near);
                m(3, 0) = -(right + left) / (right - left);
                m(3, 1) = -(top + bottom) / (top - bottom);
                m(3, 2) = -near / (far - near);
            }
        } else {
            if constexpr (Hand == Hand::Left) {
                m(2, 2) = Type(2) / (far - near);
                m(3, 0) = -(right + left) / (right - left);
                m(3, 1) = -(top + bottom) / (top - bottom);
                m(3, 2) = -(far + near) / (far - near);
            } else {
                m(2, 2) = -Type(2) / (far - near);
                m(3, 0) = -(right + left) / (right - left);
                m(3, 1) = -(top + bottom) / (top - bottom);
                m(3, 2) = -(far + near) / (far - near);
            }
        }

        if constexpr (Format == ProjectionFormat::Vulkan) {
            m(1, 1) *= -1;
        }

        return m;
    }

    template<size_t Columns, size_t Rows, typename Type, typename Allocator>
    template<Hand Hand, ProjectionFormat Format>
    Mat<Columns, Rows, Type, Allocator>
    Mat<Columns, Rows, Type, Allocator>::perspective(Type fovY,
                                                     Type aspectRatio,
                                                     Type near,
                                                     Type far) requires (
    Columns == 4 && Rows == 4) {
        Type top = std::tan(fovY / Type(2)) * near;
        Type right = top * aspectRatio;
        return frustum(-right, right, -top, top, near, far);
    }

    template<size_t Columns, size_t Rows, typename Type, typename Allocator>
    template<Hand Hand>
    Mat<Columns, Rows, Type, Allocator>
    Mat<Columns, Rows, Type, Allocator>::infinitePerspective(
            Type fovY, Type aspectRatio, Type near) requires (
    Columns == 4 && Rows == 4) {
        Type top = std::tan(fovY / Type(2)) * near;
        Type right = top * aspectRatio;

        Mat m = Mat();
        m(0, 0) = near / right;
        m(1, 1) = near / top;

        m(3, 2) = -Type(2) * near;

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
