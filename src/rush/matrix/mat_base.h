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

    template<size_t Columns, size_t Rows, typename Type,
            typename Allocator = StaticAllocator<Columns, rush::Vec<Rows, Type>>>
    struct Mat {

        static_assert(Columns > 0, "Column amount cannot be zero.");
        static_assert(Rows > 0, "Row amount cannot be zero.");

        static_assert(
                std::is_same_v<typename Allocator::AllocType, rush::Vec<Rows, Type>>,
                "Allocator type is not the same as the matrix type.");
        static_assert(Columns == Allocator::size(),
                      "Allocator size is not the same as the matrix size.");

        using Self = Mat<Columns, Rows, Type>;
        using ColumnType = rush::Vec<Rows, Type>;

        Allocator data;

        template<typename... T>
        requires (std::is_convertible_v<std::common_type_t<T...>, Type>
                  && sizeof...(T) <= Columns * Rows)
        Mat(T... list);

        Mat();

        explicit Mat(Type diagonal);

        explicit Mat(std::function<Type(size_t, size_t)> populator);

        explicit Mat(std::function<Type(size_t, size_t, size_t, size_t)>
                     populator);

        constexpr size_t size() const;

        inline const Type* toPointer() const;

        inline Type* toPointer();

        // REGION ACCESSORS

        inline ColumnType& column(size_t column);
        inline const ColumnType& column(size_t column) const;

        VecRef<Columns, Type> row(size_t row);
        Vec<Columns, Type, StaticAllocator<Columns, Type>>
        row(size_t row) const;

        inline ColumnType& operator[](size_t column);
        inline const ColumnType& operator[](size_t column) const;

        inline Type& operator()(size_t column, size_t row);
        inline const Type& operator()(size_t column, size_t row) const;

        // REGION OPERATORS

        // UNARY

        Type determinant() const requires HasAdd<Type> &&
                                          HasSub<Type> &&
                                          HasMul<Type> &&
                                          HasDiv<Type> &&
                                          (Columns == Rows);

        Mat<Rows, Columns, Type> transpose() const;

        Self inverse() const requires HasAdd<Type> &&
                                      HasSub<Type> &&
                                      HasMul<Type> &&
                                      HasDiv<Type> &&
                                      (Columns == Rows) &&
                                      (Columns < 5);

        inline Self& operator+();
        inline Self& operator+() const;
        inline Self operator-() const requires HasSub<Type>;


        // ASSIGN MATRIX - SCALE

        inline Self& operator+=(const Type& s) requires HasAdd<Type>;
        inline Self& operator-=(const Type& s) requires HasSub<Type>;
        inline Self& operator*=(const Type& s) requires HasMul<Type>;
        inline Self& operator/=(const Type& s) requires HasDiv<Type>;
        inline Self& operator<<=(const Type& s) requires HasShl<Type>;
        inline Self& operator>>=(const Type& s) requires HasShr<Type>;
        inline Self& operator&=(const Type& s) requires HasBitAnd<Type>;
        inline Self& operator|=(const Type& s) requires HasBitOr<Type>;
        inline Self& operator^=(const Type& s) requires HasBitXor<Type>;

        // ASSIGN VECTOR - VECTOR

        template<typename OAlloc>
        inline Mat& operator+=(
                const Mat<Columns, Rows, Type, OAlloc>& o) requires HasAdd<Type>;

        template<typename OAlloc>
        inline Mat& operator-=(
                const Mat<Columns, Rows, Type, OAlloc>& o)requires HasSub<Type>;

        // MATRIX - SCALE

        inline Self operator+(const Type& s) const requires HasAdd<Type>;
        inline Self operator-(const Type& s) const requires HasSub<Type>;
        inline Self operator*(const Type& s) const requires HasMul<Type>;
        inline Self operator/(const Type& s) const requires HasDiv<Type>;
        inline Self operator<<(const Type& s) const requires HasShl<Type>;
        inline Self operator>>(const Type& s) const requires HasShr<Type>;
        inline Self operator&(const Type& s) const requires HasBitAnd<Type>;
        inline Self operator|(const Type& s) const requires HasBitOr<Type>;
        inline Self operator^(const Type& s) const requires HasBitXor<Type>;
        inline Self operator&&(const Type& s) const requires HasAnd<Type>;
        inline Self operator||(const Type& s) const requires HasOr<Type>;

        // MATRIX - MATRIX

        template<typename OAlloc>
        inline Mat operator+(const Mat<Columns, Rows, Type, OAlloc>& other)
        const requires HasAdd<Type>;

        template<typename OAlloc>
        inline Mat operator-(const Mat<Columns, Rows, Type, OAlloc>& other)
        const requires HasSub<Type>;

        template<size_t OC, size_t OR, typename OAlloc = Allocator>
        rush::Mat<OC, Rows, Type>
        operator*(const rush::Mat<OC, OR, Type, OAlloc>& other) const requires
        (Columns == OR && HasAdd<Type> && HasMul<Type>);

        template<typename OAlloc>
        inline bool
        operator==(const Mat<Columns, Rows, Type, OAlloc>& other) const;

        template<typename OAlloc>
        inline bool
        operator!=(const Mat<Columns, Rows, Type, OAlloc>& other) const;

        // ENDREGION

        // REGION ITERATOR

        inline auto begin();
        inline auto end();
        inline auto cbegin() const;
        inline auto cend() const;
        inline auto rbegin();
        inline auto rend();
        inline auto crbegin() const;
        inline auto crend() const;

        // ENDREGION

    };

}

#include <rush/matrix/mat_impl.h>

#endif //RUSH_MAT_BASE_H
