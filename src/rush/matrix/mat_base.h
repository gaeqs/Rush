//
// Created by gaelr on 03/08/2023.
//

#ifndef RUSH_MAT_BASE_H
#define RUSH_MAT_BASE_H

#include <functional>
#include <utility>
#include <algorithm>

#include <rush/algorithm.h>
#include <rush/vector/vec.h>
#include <rush/matrix/mat_dense_rep.h>

#ifdef RUSH_GLM

#include <glm/glm.hpp>

#endif

namespace rush {
    template<typename Type>
    struct Quat;

    template<size_t Columns, size_t Rows, typename Type,
        typename Representation = MatDenseRep,
        typename Allocator = StaticAllocator>
    struct Mat {
        static_assert(Columns > 0, "Column amount cannot be zero.");
        static_assert(Rows > 0, "Row amount cannot be zero.");

        using Self = Mat<Columns, Rows, Type>;
        using ColumnType = rush::Vec<Rows, Type>;
        using Rep = typename Representation::
        template Representation<Columns, Rows, Type, Allocator>;

        Rep rep;

        template<typename... T>
            requires (std::is_convertible_v<std::common_type_t<T...>, Type>
                      && sizeof...(T) <= Columns * Rows && sizeof...(T) > 1)
        Mat(T... list);

        Mat();

        explicit Mat(Type diagonal);

        explicit Mat(std::function<Type(size_t, size_t)> populator);

        explicit Mat(std::function<Type(size_t, size_t, size_t, size_t)>
            populator);

        template<size_t OColumns, size_t ORows, typename ORep, typename OAlloc>
            requires(Columns > OColumns || Rows > ORows)
        explicit
        Mat(const Mat<OColumns, ORows, Type, ORep, OAlloc>& other, Type diagonal);

        [[nodiscard]] constexpr size_t size() const;

        const Type* toPointer() const;

        Type* toPointer();

        // REGION ACCESSORS

        typename Rep::ColumnRef column(size_t column);

        typename Rep::ColumnType column(size_t column) const;

        typename Rep::RowRef row(size_t row);

        typename Rep::RowType row(size_t row) const;

        typename Rep::ColumnRef operator[](size_t column);

        typename Rep::ColumnType operator[](size_t column) const;

        Type& operator()(size_t column, size_t row);

        const Type& operator()(size_t column, size_t row) const;

        // REGION OPERATORS

        // UNARY

        Type determinant() const requires HasAdd<Type> &&
                                          HasSub<Type> &&
                                          HasMul<Type> &&
                                          HasDiv<Type> &&
                                          (Columns == Rows);

        Mat<Rows, Columns, Type, Representation, Allocator>
        transpose() const;

        Self inverse() const requires HasAdd<Type> &&
                                      HasSub<Type> &&
                                      HasMul<Type> &&
                                      HasDiv<Type> &&
                                      (Columns == Rows) &&
                                      (Columns < 5);

        template<typename To, typename OAlloc = Allocator>
        Mat<Columns, Rows, To, OAlloc> cast() const;

        Self& operator+();

        Self& operator+() const;

        Self operator-() const requires HasSub<Type>;


        // ASSIGN MATRIX - SCALE

        Self& operator+=(const Type& s) requires HasAdd<Type>;

        Self& operator-=(const Type& s) requires HasSub<Type>;

        Self& operator*=(const Type& s) requires HasMul<Type>;

        Self& operator/=(const Type& s) requires HasDiv<Type>;

        Self& operator<<=(const Type& s) requires HasShl<Type>;

        Self& operator>>=(const Type& s) requires HasShr<Type>;

        Self& operator&=(const Type& s) requires HasBitAnd<Type>;

        Self& operator|=(const Type& s) requires HasBitOr<Type>;

        Self& operator^=(const Type& s) requires HasBitXor<Type>;

        // ASSIGN VECTOR - VECTOR

        template<typename OAlloc>
        Mat& operator+=(
            const Mat<Columns, Rows, Type, OAlloc>& o) requires HasAdd<Type>;

        template<typename OAlloc>
        Mat& operator-=(
            const Mat<Columns, Rows, Type, OAlloc>& o) requires HasSub<Type>;

        // MATRIX - SCALE

        Self operator+(const Type& s) const requires HasAdd<Type>;

        Self operator-(const Type& s) const requires HasSub<Type>;

        Self operator*(const Type& s) const requires HasMul<Type>;

        Self operator/(const Type& s) const requires HasDiv<Type>;

        Self operator<<(const Type& s) const requires HasShl<Type>;

        Self operator>>(const Type& s) const requires HasShr<Type>;

        Self operator&(const Type& s) const requires HasBitAnd<Type>;

        Self operator|(const Type& s) const requires HasBitOr<Type>;

        Self operator^(const Type& s) const requires HasBitXor<Type>;

        Self operator&&(const Type& s) const requires HasAnd<Type>;

        Self operator||(const Type& s) const requires HasOr<Type>;

        // MATRIX - VECTOR

        template<typename OAlloc = Allocator>
        Vec<Columns, Type, Allocator>
        operator*(const Vec<Columns, Type, OAlloc>& other) const requires
            (HasAdd<Type> && HasMul<Type>);

        // MATRIX - MATRIX

        template<typename ORep, typename OAlloc>
        Mat operator+(
            const Mat<Columns, Rows, Type, ORep, OAlloc>& other)
        const requires HasAdd<Type>;

        template<typename ORep, typename OAlloc>
        Mat operator-(
            const Mat<Columns, Rows, Type, ORep, OAlloc>& other)
        const requires HasSub<Type>;

        template<size_t OC, size_t OR,
            typename ORep = MatDenseRep,
            typename OAlloc = Allocator>
        Mat<OC, Rows, Type, Representation, Allocator>
        operator*(const rush::Mat<OC, OR, Type, ORep, OAlloc>& other) const
            requires
            (Columns == OR && HasAdd<Type> && HasMul<Type>);

        template<typename OAlloc>
        bool
        operator==(const Mat<Columns, Rows, Type, OAlloc>& other) const;

        template<typename OAlloc>
        bool
        operator!=(const Mat<Columns, Rows, Type, OAlloc>& other) const;

        // ENDREGION

        // REGION ITERATOR

        auto begin();

        auto end();

        auto cbegin() const;

        auto cend() const;

        auto rbegin();

        auto rend();

        auto crbegin() const;

        auto crend() const;

        // ENDREGION

        // REGION STATIC CREATION

        /**
         * Creates a translation matrix that moves
         * points the given amount of units.
         * @param t the amount of units.
         * @return the translation matrix.
         */
        static Mat
        translate(const rush::Vec<3, Type>& t) requires (
            Columns == 4 && Rows == 4);

        /**
         * Creates a scale matrix that scales
         * points the given amount of units.
         * @param s the given mount of units.
         * @return the scale matrix.
         */
        static Mat
        scale(const rush::Vec<3, Type>& s) requires (Columns == 4 && Rows == 4);

        /**
         * Creates a rotation matrix that rotates
         * points the given amount of radians in the
         * X axis.
         * @param radians the angle.
         * @return the rotation matrix.
         */
        static Mat
        rotationX(Type radians) requires (Columns == 4 && Rows == 4);

        /**
         * Creates a rotation matrix that rotates
         * points the given amount of radians in the
         * Y axis.
         * @param radians the angle.
         * @return the rotation matrix.
         */
        static Mat
        rotationY(Type radians) requires (Columns == 4 && Rows == 4);

        /**
         * Creates a rotation matrix that rotates
         * points the given amount of radians in the
         * X axis.
         * @param radians the angle.
         * @return the rotation matrix.
         */
        static Mat
        rotationZ(Type radians) requires (Columns == 4 && Rows == 4);

        /**
         * Creates a model matrix that transforms points
         * using the given scale, rotation and translation.
         * @param s the scale.
         * @param r the rotation.
         * @param t the translation.
         * @return the model matrix.
         */
        static Mat
        model(const rush::Vec<3, Type>& s,
              const rush::Quat<Type>& r,
              const rush::Vec<3, Type>& t) requires (
            Columns == 4 && Rows == 4);

        /**
         * Creates a normal matrix that transforms normals
         * using the given scale, rotation and translation.
         * @param s the scale.
         * @param r the rotation.
         * @return the model matrix.
         */
        static Mat
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
        static Mat lookAt(
            const rush::Vec<3, Type>& origin,
            const rush::Vec<3, Type>& direction,
            const rush::Vec<3, Type>& up) requires (
            Columns == 4 && Rows == 4);

        template<Hand Hand = Hand::Right,
            ProjectionFormat Format = ProjectionFormat::OpenGL>
        static Mat frustum(Type left, Type right,
                           Type bottom, Type top,
                           Type near, Type far) requires (
            Columns == 4 && Rows == 4);

        template<Hand Hand = Hand::Right,
            ProjectionFormat Format = ProjectionFormat::OpenGL>
        static Mat orthogonal(Type left, Type right,
                              Type bottom, Type top,
                              Type near, Type far) requires (
            Columns == 4 && Rows == 4);

        template<Hand Hand = Hand::Right,
            ProjectionFormat Format = ProjectionFormat::OpenGL>
        static Mat perspective(Type fovY, Type aspectRatio,
                               Type near, Type far) requires (
            Columns == 4 && Rows == 4);

        template<Hand Hand = Hand::Right>
        static Mat infinitePerspective(Type fovY, Type aspectRatio,
                                       Type near) requires (
            Columns == 4 && Rows == 4);

        // ENDREGION

        // REGION GLM SUPPORT

#ifdef RUSH_GLM

        Mat(const glm::mat<Columns, Rows, Type>& o) {
            for(size_t c = 0; c < Columns; ++c) {
                for(size_t r = 0; r < Columns; ++r) {
                    operator()(c, r) = o[c][r];
                }
            }
        }

        Mat(const glm::mat<Columns, Rows, Type>&& o) {
            for(size_t c = 0; c < Columns; ++c) {
                for(size_t r = 0; r < Columns; ++r) {
                    operator()(c, r) = std::move(o[c][r]);
                }
            }
        }

        operator glm::mat<Columns, Rows, Type>() const {
            glm::mat<Columns, Rows, Type> result;
            for(size_t c = 0; c < Columns; ++c) {
                for(size_t r = 0; r < Columns; ++r) {
                    result[c][r] = operator()(c, r);
                }
            }
            return result;
        }

#endif

        // ENDREGION
    };
}

#include <rush/matrix/mat_impl.h>

#endif //RUSH_MAT_BASE_H
