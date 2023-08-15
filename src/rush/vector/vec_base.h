//
// Created by gaelr on 31/07/2023.
//

#ifndef RUSH_VEC_BASE_H
#define RUSH_VEC_BASE_H

#include <utility>
#include <functional>
#include <algorithm>
#include <string>
#include <iterator>
#include <array>

#include <rush/concepts.h>
#include <rush/vector/vec_ref.h>
#include <rush/algorithm.h>

namespace rush {

    template<size_t Size,
            typename Type,
            typename Allocator = StaticAllocator> requires (Size > 0)
    struct Vec {

        using Storage = Allocator;

        Allocator::template AllocatedData<Size, Type> data;

        template<typename... T>
        requires std::is_convertible_v<std::common_type_t<T...>, Type> &&
                 (sizeof...(T) <= Size)
        Vec(T... list);

        Vec();

        template<size_t OSize, typename OAlloc>
        requires(Size < OSize)
        Vec(const Vec<OSize, Type, OAlloc>& other);

        explicit Vec(const VecRef<Size, Type>& ref);

        explicit Vec(std::function<Type(size_t)> populator);

        explicit Vec(std::function<Type(size_t, size_t)> populator);

        /**
         * Returns a pointer to the first element of this
         * vector.
         * All other elements of the vector are contiguous
         * to this pointer.
         * @return the pointer.
         */
        [[nodiscard]] inline Type* toPointer();

        /**
         * Returns a pointer to the first element of this
         * vector.
         * All other elements of the vector are contiguous
         * to this pointer.
         * @return the pointer.
         */
        [[nodiscard]] inline const Type* toPointer() const;

        /**
         * Returns the size of this vector.
         * @return the size.
         */
        [[nodiscard]] constexpr size_t size() const;

        // REGION ACCESSORS

        inline Type& x();
        inline Type& y() requires (Size >= 2);
        inline Type& z() requires (Size >= 3);
        inline Type& w() requires (Size >= 4);
        inline const Type& x() const;
        inline const Type& y() const requires (Size >= 2);
        inline const Type& z() const requires (Size >= 3);
        inline const Type& w() const requires (Size >= 4);

        inline Type& operator[](size_t index);
        inline const Type& operator[](size_t index) const;

        /**
         * Returns a vector referencing the given indices
         * of this vector.
         * <p>
         * This method can be used to swizzle the values
         * of the vector.
         *
         * @example
         *
         * rush::Vec<3, float> a = {2, 4, 6};
         * <p>
         * rush::Vec<2, float> b = a(2, 1); // b == {6, 4}
         * <p>
         * rush::Vec<5, float> c = a(0, 0, 1, 0, 0); // c == {2, 2, 4, 2, 2}
         * <p>
         * a(0, 1) = a(2, 0); // a == {6, 2, 6}
         *
         * @tparam Ts the packed type of the index.
         * All them must be convertible to size_t.
         * @param indices the indices.
         * @return the vector reference.
         */
        template<typename... Ts>
        requires std::is_convertible_v<std::common_type_t<Ts...>, size_t>
        VecRef<sizeof...(Ts), Type>
        operator()(Ts&& ... indices);

        /**
         * Returns a vector with the data at the given indices
         * of this vector.
         * <p>
         * This method can be used to swizzle the values
         * of the vector.
         *
         * @example
         *
         * rush::Vec<3, float> a = {2, 4, 6};
         * <p>
         * rush::Vec<2, float> b = a(2, 1); // b == {6, 4}
         * <p>
         * rush::Vec<5, float> c = a(0, 0, 1, 0, 0); // c == {2, 2, 4, 2, 2}
         * <p>
         * a(0, 1) = a(2, 0); // a == {6, 2, 6}, not valid if 'a' is const!
         *
         * @tparam Ts the packed type of the index.
         * All them must be convertible to size_t.
         * @param indices the indices.
         * @return the new vector.
         */
        template<typename... Ts, typename OAlloc = Allocator>
        requires std::is_convertible_v<std::common_type_t<Ts...>, size_t>
        Vec<sizeof...(Ts), Type, OAlloc>
        operator()(Ts&& ... indices) const;

        // ENDREGION

        // REGION OPERATIONS

        /**
         * Returns the squared length (or squared modulus) of this vector.
         * The squared length is defined the as the sum of all
         * entries of this vector multiplied by themselves.
         *
         * @return the squared length.
         */
        inline Type squaredLength() const;

        /**
         * Returns the length (or modulus) of this vector.
         * The length is defined the as the squared root of the sum of all
         * entries of this vector multiplied by themselves.
         *
         * @return the length.
         */
        template<typename Return = Type>
        inline Return length() const requires (
        std::is_convertible_v<Type, Return> && HasSquaredRoot<Type>);

        /**
         * Returns the inverse of the length (or modulus) of this vector.
         * The length is defined the as the squared root of the sum of all
         * entries of this vector multiplied by themselves.
         * <p>
         * You can select the precision of the algorithm.
         * A high precision algorithm will provide a high precision
         * result in exchange of more computational cost.
         * A low precision algorithm will be faster, but it may provide
         * poorer results.
         * <p>
         * You can also select whether the algorithm will use pure C++ code
         * or it will try to use intrinsic processor instructions.
         *
         * @return the length.
         *
         */
        template<typename Return = Type, Algorithm Algorithm = Algorithm()>
        Return inverseLength() const requires (
        std::is_convertible_v<Type, Return> && HasSquaredRoot<Type>);

        /**
         * Returns a normalized version of this vector.
         * <p>
         * You can select the precision of the algorithm.
         * A high precision algorithm will provide a high precision
         * result in exchange of more computational cost.
         * A low precision algorithm will be faster, but it may provide
         * poorer results.
         * <p>
         * You can also select whether the algorithm will use pure C++ code
         * or it will try to use intrinsic processor instructions.
         *
         * @return the normalized vector.
         *
         */
        template<typename Return = Type, Algorithm Algorithm = Algorithm(),
                typename OAlloc = StaticAllocator>
        Vec<Size, Return, OAlloc> normalized() const requires HasMul<Return>;

        // UNARY

        inline Vec& operator+();
        inline const Vec& operator+() const;
        inline Vec operator-() const requires HasSub<Type>;

        // ASSIGN VECTOR - SCALE

        inline Vec& operator+=(const Type& s) requires HasAdd<Type>;
        inline Vec& operator-=(const Type& s) requires HasSub<Type>;
        inline Vec& operator*=(const Type& s) requires HasMul<Type>;
        inline Vec& operator/=(const Type& s) requires HasDiv<Type>;
        inline Vec& operator<<=(const Type& s) requires HasShl<Type>;
        inline Vec& operator>>=(const Type& s) requires HasShr<Type>;
        inline Vec& operator&=(const Type& s) requires HasBitAnd<Type>;
        inline Vec& operator|=(const Type& s) requires HasBitOr<Type>;
        inline Vec& operator^=(const Type& s) requires HasBitXor<Type>;

        // ASSIGN VECTOR - VECTOR

        template<typename OAlloc>
        inline Vec&
        operator+=(const Vec<Size, Type, OAlloc>& o) requires HasAdd<Type>;

        template<typename OAlloc>
        inline Vec&
        operator-=(const Vec<Size, Type, OAlloc>& o) requires HasSub<Type>;

        template<typename OAlloc>
        inline Vec&
        operator*=(const Vec<Size, Type, OAlloc>& o) requires HasMul<Type>;

        template<typename OAlloc>
        inline Vec&
        operator/=(const Vec<Size, Type, OAlloc>& o) requires HasDiv<Type>;

        template<typename OAlloc>
        inline Vec&
        operator<<=(const Vec<Size, Type, OAlloc>& o) requires HasShl<Type>;

        template<typename OAlloc>
        inline Vec&
        operator>>=(const Vec<Size, Type, OAlloc>& o) requires HasShr<Type>;

        template<typename OAlloc>
        inline Vec&
        operator&=(const Vec<Size, Type, OAlloc>& o) requires HasBitAnd<Type>;

        template<typename OAlloc>
        inline Vec&
        operator|=(const Vec<Size, Type, OAlloc>& o) requires HasBitOr<Type>;

        template<typename OAlloc>
        inline Vec&
        operator^=(const Vec<Size, Type, OAlloc>& o) requires HasBitXor<Type>;

        // VECTOR - SCALE

        inline Vec operator+(const Type& s) const requires HasAdd<Type>;
        inline Vec operator-(const Type& s) const requires HasSub<Type>;
        inline Vec operator*(const Type& s) const requires HasMul<Type>;
        inline Vec operator/(const Type& s) const requires HasDiv<Type>;
        inline Vec operator<<(const Type& s) const requires HasShl<Type>;
        inline Vec operator>>(const Type& s) const requires HasShr<Type>;
        inline Vec operator&(const Type& s) const requires HasBitAnd<Type>;
        inline Vec operator|(const Type& s) const requires HasBitOr<Type>;
        inline Vec operator^(const Type& s) const requires HasBitXor<Type>;
        inline Vec operator&&(const Type& s) const requires HasAnd<Type>;
        inline Vec operator||(const Type& s) const requires HasOr<Type>;

        // VECTOR - VECTOR
        template<typename OAlloc>
        inline Vec operator+(const Vec<Size, Type, OAlloc>& other)
        const requires HasAdd<Type>;

        template<typename OAlloc>
        inline Vec operator-(const Vec<Size, Type, OAlloc>& other)
        const requires HasSub<Type>;

        template<typename OAlloc>
        inline Vec operator*(const Vec<Size, Type, OAlloc>& other)
        const requires HasMul<Type>;

        template<typename OAlloc>
        inline Vec operator/(const Vec<Size, Type, OAlloc>& other)
        const requires HasDiv<Type>;

        template<typename OAlloc>
        inline Vec operator<<(const Vec<Size, Type, OAlloc>& other)
        const requires HasShl<Type>;

        template<typename OAlloc>
        inline Vec operator>>(const Vec<Size, Type, OAlloc>& other)
        const requires HasShr<Type>;

        template<typename OAlloc>
        inline Vec operator&&(const Vec<Size, Type, OAlloc>& other)
        const requires HasAnd<Type>;

        template<typename OAlloc>
        inline Vec operator||(const Vec<Size, Type, OAlloc>& other)
        const requires HasOr<Type>;

        // DOT
        template<typename OAlloc>
        inline Type dot(const Vec<Size, Type, OAlloc>& other) const;

        template<typename OAlloc>
        inline Type
        operator%(const Vec<Size, Type, OAlloc>& other) const requires (
        HasAdd<Type> && HasMul<Type>);

        // CROSS
        template<typename OAlloc>
        inline Vec cross(const Vec<Size, Type, OAlloc>& other) const requires (
        Size == 3 && HasAdd<Type> && HasMul<Type>);

        template<typename OAlloc>
        inline Vec operator^(const Vec<Size, Type, OAlloc>& o) const requires (
        Size == 3 && HasAdd<Type> && HasMul<Type>);

        template<typename OAlloc>
        inline bool operator==(const Vec<Size, Type, OAlloc>& other) const;

        template<typename OAlloc>
        inline bool operator!=(const Vec<Size, Type, OAlloc>& other) const;

        inline bool operator==(const VecRef<Size, Type>& other) const;
        inline bool operator!=(const VecRef<Size, Type>& other) const;

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

#include <rush/vector/vec_impl.h>

#endif //RUSH_VEC_BASE_H
