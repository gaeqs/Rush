//
// Created by gaelr on 03/08/2023.
//

#ifndef RUSH_MAT_BASE_H
#define RUSH_MAT_BASE_H

#include <functional>
#include <utility>
#include <algorithm>
#include <string>
#include <stdexcept>

#include <rush/vector/vec.h>

namespace rush {

    template<size_t C, size_t R, typename Type>
    struct Mat {
        static constexpr size_t Columns = C;
        static constexpr size_t Rows = R;

        static_assert(Columns > 0, "Column amount cannot be zero.");
        static_assert(Rows > 0, "Row amount cannot be zero.");

        using Self = Mat<Columns, Rows, Type>;
        using ColumnType = rush::Vec<R, Type>;

        ColumnType data[Columns];

        template<typename... T>
        requires (std::is_convertible_v<std::common_type_t<T...>, Type>
                  && sizeof...(T) <= Columns * Rows)
        Mat(T... list) {
            Type* ptr = toPointer();
            ((*ptr++ = list), ...);
        }

        Mat() : data() {

        }

        explicit Mat(Type diagonal) : data() {
            for (size_t i = 0; i < std::min(Columns, Rows); ++i) {
                data[i][i] = diagonal;
            }
        }

        explicit Mat(std::function<Type(size_t, size_t)> populator) {
            for (size_t c = 0; c < Columns; ++c) {
                for (size_t r = 0; r < Rows; ++r) {
                    data[c][r] = populator(c, r);
                }
            }
        }

        explicit Mat(std::function<Type(size_t, size_t, size_t, size_t)>
                     populator) {
            for (size_t c = 0; c < Columns; ++c) {
                for (size_t r = 0; r < Rows; ++r) {
                    data[c][r] = populator(c, r, Columns, Rows);
                }
            }
        }

        [[nodiscard]] inline constexpr size_t size() const {
            return Columns * Rows;
        }

        inline const Type* toPointer() const {
            return data[0].toPointer();
        }

        inline Type* toPointer() {
            return data[0].toPointer();
        }

        // REGION ACCESSORS

        inline const ColumnType& column(size_t column) const {
            return (*this)[column];
        }

        inline ColumnType& column(size_t column) {
            return (*this)[column];
        }

        Vec<Columns, Type> row(size_t row) const {
            Vec<Columns, Type> vec;
            for (size_t i = 0; i < Columns; ++i) {
                vec[i] = data[i][row];
            }
            return vec;
        }

        VecRef<Columns, Type> row(size_t row) {
            VecRef<Columns, Type> vec;
            for (size_t i = 0; i < Columns; ++i) {
                vec.references[i] = &data[i][row];
            }
            return vec;
        }

        const ColumnType& operator[](size_t column) const {
            if (column >= Columns)
                throw std::out_of_range("Range is between 0 and "
                                        + std::to_string(Columns));
            return data[column];
        }

        ColumnType& operator[](size_t column) {
            if (column >= Columns)
                throw std::out_of_range("Range is between 0 and "
                                        + std::to_string(Columns));
            return data[column];
        }

        const Type& operator()(size_t column, size_t row) const {
            if (column >= Columns)
                throw std::out_of_range("Column index must be between 0 and "
                                        + std::to_string(Columns));
            if (row >= Rows)
                throw std::out_of_range("Row index must be between 0 and "
                                        + std::to_string(Rows));

            return data[column][row];
        }

        // REGION OPERATORS

        // UNARY

        Type determinant() const requires HasAdd<Type> &&
                                          HasSub<Type> &&
                                          HasMul<Type> &&
                                          HasDiv<Type> &&
                                          (Columns == Rows) {
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

        Mat<Rows, Columns, Type> transpose() const {
            return Mat{[this](size_t c, size_t r) { return operator()(r, c); }};
        }

        Self inverse() const requires HasAdd<Type> &&
                                      HasSub<Type> &&
                                      HasMul<Type> &&
                                      HasDiv<Type> &&
                                      (Columns == Rows) &&
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

        inline Self& operator+() {
            return *this;
        }

        inline Self& operator+() const {
            return *this;
        }

        inline Self operator-() const requires HasSub<Type> {
            Self result;
            for (size_t i = 0; i < Columns; ++i) {
                result[i] = -data[i];
            }
            return result;
        }

        // MATRIX - SCALE

        inline Self operator+(const Type& s) const requires
        HasAdd<Type> {
            Self result;
            for (size_t i = 0; i < Columns; ++i) {
                result[i] = data[i] + s;
            }
            return result;
        }

        inline Self operator-(const Type& s) const requires
        HasSub<Type> {
            Self result;
            for (size_t i = 0; i < Columns; ++i) {
                result[i] = data[i] - s;
            }
            return result;
        }

        inline Self operator*(const Type& s) const requires
        HasMul<Type> {
            Self result;
            for (size_t i = 0; i < Columns; ++i) {
                result[i] = data[i] * s;
            }
            return result;
        }

        inline Self operator/(const Type& s) const requires
        HasDiv<Type> {
            Self result;
            for (size_t i = 0; i < Columns; ++i) {
                result[i] = data[i] / s;
            }
            return result;
        }

        // MATRIX - MATRIX

        inline Self operator+(const Self& other) const requires
        HasAdd<Type> {
            Self result;
            for (size_t i = 0; i < Columns; ++i) {
                result[i] = data[i] + other[i];
            }
            return result;
        }

        inline Self operator-(const Self& other) const requires
        HasSub<Type> {
            Self result;
            for (size_t i = 0; i < Columns; ++i) {
                result[i] = data[i] - other[i];
            }
            return result;
        }

        template<size_t OC, size_t OR>
        rush::Mat<OC, R, Type>
        operator*(const rush::Mat<OC, OR, Type>& other) const requires
        (C == OR && HasAdd<Type> && HasMul<Type>) {
            return rush::Mat<OC, R, Type>([&](size_t c, size_t r) {
                return this->row(r) % other.column(c);
            });
        }

        inline bool operator==(const Self& other) const {
            if (this == &other) return true;
            return std::memcmp(data, other.data, sizeof(data)) == 0;
        }

        inline bool operator!=(const Self& other) const {
            if (this == &other) return false;
            return std::memcmp(data, other.data, sizeof(data)) != 0;
        }

        // ENDREGION
    };
}

#endif //RUSH_MAT_BASE_H
