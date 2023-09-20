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

#include <rush/algorithm.h>
#include <rush/vector/vec.h>

namespace rush {

    template<typename Type>
    struct Quat;

    template<size_t Columns, size_t Rows, typename Type,
            typename Allocator = StaticAllocator>
    struct Mat {

        static_assert(Columns > 0, "Column amount cannot be zero.");
        static_assert(Rows > 0, "Row amount cannot be zero.");

        using Self = Mat<Columns, Rows, Type>;
        using ColumnType = rush::Vec<Rows, Type>;

        Allocator::template AllocatedData<Columns, rush::Vec<Rows, Type>> data;

        template<typename... T>
        requires (std::is_convertible_v<std::common_type_t<T...>, Type>
                  && sizeof...(T) <= Columns * Rows)
        Mat(T... list);

        Mat();

        explicit Mat(Type diagonal);

        explicit Mat(std::function<Type(size_t, size_t)> populator);

        explicit Mat(std::function<Type(size_t, size_t, size_t, size_t)>
                     populator);

        template<size_t OColumns, size_t ORows, typename OAlloc>
        requires(Columns > OColumns || Rows > ORows)
        explicit Mat(const Mat<OColumns, ORows, Type, OAlloc>& other,
                     Type diagonal);

        constexpr size_t size() const;

        inline const Type* toPointer() const;

        inline Type* toPointer();

        // REGION ACCESSORS

        inline ColumnType& column(size_t column);

        inline const ColumnType& column(size_t column) const;

        VecRef<Columns, Type> row(size_t row);

        Vec<Columns, Type, StaticAllocator>
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

        Mat<Rows, Columns, Type, Allocator> transpose() const;

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

        // MATRIX - VECTOR

        template<typename OAlloc = Allocator>
        Vec<Columns, Type, Allocator>
        operator*(const rush::Vec<Columns, Type, OAlloc>& other) const requires
        (HasAdd<Type> && HasMul<Type>);

        // MATRIX - MATRIX

        template<typename OAlloc>
        inline Mat operator+(const Mat<Columns, Rows, Type, OAlloc>& other)
        const requires HasAdd<Type>;

        template<typename OAlloc>
        inline Mat operator-(const Mat<Columns, Rows, Type, OAlloc>& other)
        const requires HasSub<Type>;

        template<size_t OC, size_t OR, typename OAlloc = Allocator>
        Mat<OC, Rows, Type, Allocator>
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

        // REGION STATIC CREATION

        /**
         * Creates a translation matrix that moves
         * points the given amount of units.
         * @param t the amount of units.
         * @return the translation matrix.
         */
        inline static Mat
        translate(const rush::Vec<3, Type>& t) requires (
        Columns == 4 && Rows == 4);

        /**
         * Creates a scale matrix that scales
         * points the given amount of units.
         * @param s the given mount of units.
         * @return the scale matrix.
         */
        inline static Mat
        scale(const rush::Vec<3, Type>& s) requires (Columns == 4 && Rows == 4);

        /**
         * Creates a rotation matrix that rotates
         * points the given amount of radians in the
         * X axis.
         * @param radians the angle.
         * @return the rotation matrix.
         */
        inline static Mat
        rotationX(Type radians) requires (Columns == 4 && Rows == 4);

        /**
         * Creates a rotation matrix that rotates
         * points the given amount of radians in the
         * Y axis.
         * @param radians the angle.
         * @return the rotation matrix.
         */
        inline static Mat
        rotationY(Type radians) requires (Columns == 4 && Rows == 4);

        /**
         * Creates a rotation matrix that rotates
         * points the given amount of radians in the
         * X axis.
         * @param radians the angle.
         * @return the rotation matrix.
         */
        inline static Mat
        rotationZ(Type radians) requires (Columns == 4 && Rows == 4);

        /**
         * Creates a model matrix that transforms points
         * using the given scale, rotation and translation.
         * @param s the scale.
         * @param r the rotation.
         * @param t the translation.
         * @return the model matrix.
         */
        inline static Mat
        model(const rush::Vec<3, Type>& s,
              const rush::Quat<Type>& r,
              const rush::Vec<3, Type>& t) requires (
        Columns == 4 && Rows == 4);

        /**
         * Creates a normal matrix that transforms normals
         * using the given scale, rotation and translation.
         * @param s the scale.
         * @param r the rotation.
         * @param t the translation.
         * @return the model matrix.
         */
        inline static Mat
        normal(const rush::Vec<3, Type>& s,
               const rush::Quat<Type>& r) requires (
        Columns == 4 && Rows == 4);

        /**
         * Creates a view matrix for a camera that at
         * the given origin that is looking at the given
         * direction.
         * @tparam Hand whether the algorithm should be right-handed or left-handed.
         * @param origin the position of the camera.
         * @param direction the direction the camera is looking at.
         * @param up the up vector of the camera.
         * @return the view matrix.
         */
        template<Hand Hand = Hand::Right>
        inline static Mat lookAt(
                const rush::Vec<3, Type>& origin,
                const rush::Vec<3, Type>& direction,
                const rush::Vec<3, Type>& up) requires (
        Columns == 4 && Rows == 4);

        template<Hand Hand = Hand::Right,
                ProjectionFormat Format = ProjectionFormat::OpenGL>
        inline static Mat frustum(Type left, Type right,
                                  Type bottom, Type top,
                                  Type near, Type far) requires (
        Columns == 4 && Rows == 4);

        template<Hand Hand = Hand::Right,
                ProjectionFormat Format = ProjectionFormat::OpenGL>
        inline static Mat orthogonal(Type left, Type right,
                                     Type bottom, Type top,
                                     Type near, Type far) requires (
        Columns == 4 && Rows == 4);

        template<Hand Hand = Hand::Right,
                ProjectionFormat Format = ProjectionFormat::OpenGL>
        inline static Mat perspective(Type fovY, Type aspectRatio,
                                      Type near, Type far) requires (
        Columns == 4 && Rows == 4);

        template<Hand Hand = Hand::Right>
        inline static Mat infinitePerspective(Type fovY, Type aspectRatio,
                                              Type near) requires (
        Columns == 4 && Rows == 4);

        // ENDREGION

    };
}

#include <rush/matrix/mat_impl.h>

#endif //RUSH_MAT_BASE_H
