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

namespace rush {

    template<size_t Size, typename Type> requires (Size > 0)
    struct Vec {

        using Self = Vec<Size, Type>;
        using Storage = std::array<Type, Size>;

        Storage data;

        template<typename... T>
        requires std::is_convertible_v<std::common_type_t<T...>, Type> &&
                 (sizeof...(T) <= Size)
        Vec(T... list);

        Vec();

        template<size_t OSize>
        requires(Size < OSize)
        Vec(const Vec<OSize, Type>& other);

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
        template<typename... Ts>
        requires std::is_convertible_v<std::common_type_t<Ts...>, size_t>
        Vec<sizeof...(Ts), Type>
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
        [[nodiscard]] Type squaredLength() const;

        /**
         * Returns the length (or modulus) of this vector.
         * The length is defined the as the squared root of the sum of all
         * entries of this vector multiplied by themselves.
         *
         * @return the length.
         */
        template<typename Return = Type>
        [[nodiscard]] Return length() const requires (
        std::is_convertible_v<Type, Return> && HasSquaredRoot<Type>);

        // UNARY

        inline Self& operator+();
        inline const Self& operator+() const;
        inline Self operator-() const requires HasSub<Type>;

        // ASSIGN VECTOR - SCALE

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

        inline Self& operator+=(const Self& o) requires HasAdd<Type>;
        inline Self& operator-=(const Self& o) requires HasSub<Type>;
        inline Self& operator*=(const Self& o) requires HasMul<Type>;
        inline Self& operator/=(const Self& o) requires HasDiv<Type>;
        inline Self& operator<<=(const Self& o) requires HasShl<Type>;
        inline Self& operator>>=(const Self& o) requires HasShr<Type>;
        inline Self& operator&=(const Self& o) requires HasBitAnd<Type>;
        inline Self& operator|=(const Self& o) requires HasBitOr<Type>;
        inline Self& operator^=(const Self& o) requires HasBitXor<Type>;

        // VECTOR - SCALE

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

        // VECTOR - VECTOR

        inline Self operator+(const Self& other) const requires HasAdd<Type>;
        inline Self operator-(const Self& other) const requires HasSub<Type>;
        inline Self operator*(const Self& other) const requires HasMul<Type>;
        inline Self operator/(const Self& other) const requires HasDiv<Type>;
        inline Self operator<<(const Self& other) const requires HasShl<Type>;
        inline Self operator>>(const Self& other) const requires HasShr<Type>;
        inline Self operator&&(const Self& other) const requires HasAnd<Type>;
        inline Self operator||(const Self& other) const requires HasOr<Type>;

        // DOT
        inline Type dot(const Self& other);

        inline Type operator%(const Self& other) const requires (
        HasAdd<Type> && HasMul<Type>);

        // CROSS
        inline Self cross(const Self& other) const requires (
        Size == 3 && HasAdd<Type> && HasMul<Type>);

        inline Self operator^(const Self& o) const requires (
        Size == 3 && HasAdd<Type> && HasMul<Type>);

        inline bool operator==(const Self& other) const;
        inline bool operator!=(const Self& other) const;

        // ENDREGION

        // REGION ITERATOR

        inline Storage::iterator begin();
        inline Storage::iterator end();
        inline Storage::const_iterator cbegin() const;
        inline Storage::const_iterator cend() const;
        inline Storage::reverse_iterator rbegin();
        inline Storage::reverse_iterator rend();
        inline Storage::const_reverse_iterator crbegin() const;
        inline Storage::const_reverse_iterator crend() const;

        // ENDREGION

    };

}

#include <rush/vector/vec_impl.h>

#endif //RUSH_VEC_BASE_H
