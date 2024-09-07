//
// Created by gaelr on 31/07/2023.
//

#ifndef RUSH_VEC_BASE_H
#define RUSH_VEC_BASE_H

#include <functional>
#include <array>

#include <rush/concepts.h>
#include <rush/vector/vec_ref.h>
#include <rush/algorithm.h>

#ifdef RUSH_GLM

#include <glm/glm.hpp>

#endif

namespace rush {
    /**
     * Represents a mathematical vector.
     * A vector can be defined as an array of values used for mathematical
     * operations.
     *
     * @tparam Size the size of the vector.
     * @tparam Type the type of the vector.
     * @tparam Allocator the allocators used to store the data of the vector.
     */
    template<size_t Size,
        typename Type,
        typename Allocator = StaticAllocator> requires (Size > 0)
    struct Vec {
        using Storage = Allocator;

        typename Allocator::template AllocatedData<Size, Type> data;

        /**
         * Creates a new empty vector.
         * <p>
         * All members of the new vector will be
         * initialized to their default values.
         */
        Vec();

        /**
         * Creates a new vector will all its members
         * initialized to the given value.
         * @param fill the value of all members.
         */
        explicit Vec(Type fill);

        /**
         * Creates a new vector will all its members
         * initialized to the given values.
         * @tparam T the type of the values.
         * @param list the values.
         */
        template<typename... T>
            requires std::is_convertible_v<std::common_type_t<T...>, Type> &&
                     (Size > 1 && sizeof...(T) == Size)
        Vec(T... list);

        /**
         * Creates a new vector with the data
         * of the given vector.
         * The new vector must be smaller than the
         * given vector.
         * @tparam OSize the size of the given vector.
         * @tparam OAlloc the allocator of the given vector.
         * @param other the given vector.
         */
        template<size_t OSize, typename OAlloc>
            requires(Size < OSize)
        Vec(const Vec<OSize, Type, OAlloc>& other);

        /**
         * Creates a new vector with the data
         * of the given vector and the values
         * of the given list.
         * @tparam OSize the size of the given vector.
         * @tparam OAlloc the allocator of the given vector.
         * @tparam T the type of the values.
         * @param other the given vector.
         * @param list the given list.
         */
        template<size_t OSize, typename OAlloc, typename... T>
            requires std::is_convertible_v<std::common_type_t<T...>, Type> &&
                     (sizeof...(T) + OSize == Size)
        Vec(const Vec<OSize, Type, OAlloc>& other, T... list);

        /**
         * Creates a new vector from the given vector
         * reference.
         * @param ref the given vector reference.
         */
        explicit Vec(const VecRef<Size, Type>& ref);

        /**
         * Creates a new vector using the given population
         * function to fill the vector.
         * The function will be called for each element,
         * giving each time the index of the element.
         * @param populator the population function.
         */
        explicit Vec(std::function<Type(size_t)> populator);

        /**
         * Creates a new vector using the given population
         * function to fill the vector.
         * The function will be called for each element,
         * giving each time the index of the element.
         * The second parameter is always the size of the vector.
         * @param populator the population function.
         */
        explicit Vec(std::function<Type(size_t, size_t)> populator);

        /**
         * Returns a pointer to the first element of this
         * vector.
         * All other elements of the vector are contiguous
         * to this pointer.
         * @return the pointer.
         */
        [[nodiscard]] Type* toPointer();

        /**
         * Returns a pointer to the first element of this
         * vector.
         * All other elements of the vector are contiguous
         * to this pointer.
         * @return the pointer.
         */
        [[nodiscard]] const Type* toPointer() const;

        /**
         * Returns the size of this vector.
         * @return the size.
         */
        [[nodiscard]] constexpr size_t size() const;

        // REGION ACCESSORS

        /**
         * Returns the first value of this vector.
         * @return the first value.
         */
        Type& x();

        /**
         * Returns the second value of this vector.
         * @return the second value.
         */
        Type& y() requires (Size >= 2);

        /**
         * Returns the third value of this vector.
         * @return the third value.
         */
        Type& z() requires (Size >= 3);

        /**
         * Returns the fourth value of this vector.
         * @return the fourth value.
         */
        Type& w() requires (Size >= 4);

        /**
         * Returns the first value of this vector.
         * @return the first value.
         */
        const Type& x() const;

        /**
         * Returns the second value of this vector.
         * @return the second value.
         */
        const Type& y() const requires (Size >= 2);

        /**
         * Returns the third value of this vector.
         * @return the third value.
         */
        const Type& z() const requires (Size >= 3);

        /**
         * Returns the fourth value of this vector.
         * @return the fourth value.
         */
        const Type& w() const requires (Size >= 4);

        /**
         * Returns the value at the given index.
         * This method doesn't check for overflows.
         * @param index the index.
         * @return the value at the given index.
         */
        Type& operator[](size_t index);

        /**
         * Returns the value at the given index.
         * This method doesn't check for overflows.
         * @param index the index.
         * @return the value at the given index.
         */
        const Type& operator[](size_t index) const;

        /**
         * Returns a vector referencing the given indices
         * of this vector.
         * <p>
         * This method can be used to swizzle the values
         * of the vector.
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
        operator()(Ts&&... indices);

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
        operator()(Ts&&... indices) const;

        // ENDREGION

        // REGION OPERATIONS

        // UNARY

        /**
         * Returns the squared length (or squared modulus) of this vector.
         * The squared length is defined the as the sum of all
         * entries of this vector multiplied by themselves.
         *
         * @return the squared length.
         */
        Type squaredLength() const;

        /**
         * Returns the length (or modulus) of this vector.
         * The length is defined the as the squared root of the sum of all
         * entries of this vector multiplied by themselves.
         *
         * @return the length.
         */
        template<typename Return = Type>
        Return length() const requires (
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
            typename OAlloc = Allocator>
        Vec<Size, Return, OAlloc> normalized() const requires HasMul<Return>;

        /**
         * Returns a copy of this vector with all its elements
         * casted to the given type.
         * @tparam To the type.
         * @tparam OAlloc the allocator used on the new vector.
         * @return the new vector.
         */
        template<typename To, typename OAlloc = Allocator>
        Vec<Size, To, OAlloc> cast() const;

        /**
         * Returns a copy of this vector with all its elements'
         * order reversed.
         * @tparam OAlloc the allocator used on the new vector.
         * @return the new vector.
         */
        template<typename OAlloc = Allocator>
        Vec<Size, Type, OAlloc> reverse() const;

        /**
         * Returns an std::array containing the elements
         * of this vector.
         *
         * \return the array.
         */
        std::array<Type, Size> toArray() const;

        Vec& operator+();

        const Vec& operator+() const;

        Vec operator-() const requires HasSub<Type>;

        // ASSIGN VECTOR - SCALE

        Vec& operator+=(const Type& s) requires HasAdd<Type>;

        Vec& operator-=(const Type& s) requires HasSub<Type>;

        Vec& operator*=(const Type& s) requires HasMul<Type>;

        Vec& operator/=(const Type& s) requires HasDiv<Type>;

        Vec& operator<<=(const Type& s) requires HasShl<Type>;

        Vec& operator>>=(const Type& s) requires HasShr<Type>;

        Vec& operator&=(const Type& s) requires HasBitAnd<Type>;

        Vec& operator|=(const Type& s) requires HasBitOr<Type>;

        Vec& operator^=(const Type& s) requires HasBitXor<Type>;

        // ASSIGN VECTOR - VECTOR

        template<typename OAlloc>
        Vec&
        operator+=(const Vec<Size, Type, OAlloc>& o) requires HasAdd<Type>;

        template<typename OAlloc>
        Vec&
        operator-=(const Vec<Size, Type, OAlloc>& o) requires HasSub<Type>;

        template<typename OAlloc>
        Vec&
        operator*=(const Vec<Size, Type, OAlloc>& o) requires HasMul<Type>;

        template<typename OAlloc>
        Vec&
        operator/=(const Vec<Size, Type, OAlloc>& o) requires HasDiv<Type>;

        template<typename OAlloc>
        Vec&
        operator<<=(const Vec<Size, Type, OAlloc>& o) requires HasShl<Type>;

        template<typename OAlloc>
        Vec&
        operator>>=(const Vec<Size, Type, OAlloc>& o) requires HasShr<Type>;

        template<typename OAlloc>
        Vec&
        operator&=(const Vec<Size, Type, OAlloc>& o) requires HasBitAnd<Type>;

        template<typename OAlloc>
        Vec&
        operator|=(const Vec<Size, Type, OAlloc>& o) requires HasBitOr<Type>;

        template<typename OAlloc>
        Vec&
        operator^=(const Vec<Size, Type, OAlloc>& o) requires HasBitXor<Type>;

        // VECTOR - SCALE

        Vec operator+(const Type& s) const requires HasAdd<Type>;

        Vec operator-(const Type& s) const requires HasSub<Type>;

        Vec operator*(const Type& s) const requires HasMul<Type>;

        Vec operator/(const Type& s) const requires HasDiv<Type>;

        Vec operator%(const Type& s) const requires HasMod<Type>;

        Vec operator<<(const Type& s) const requires HasShl<Type>;

        Vec operator>>(const Type& s) const requires HasShr<Type>;

        Vec operator&(const Type& s) const requires HasBitAnd<Type>;

        Vec operator|(const Type& s) const requires HasBitOr<Type>;

        Vec operator^(const Type& s) const requires HasBitXor<Type>;

        Vec operator&&(const Type& s) const requires HasAnd<Type>;

        Vec operator||(const Type& s) const requires HasOr<Type>;

        // VECTOR - VECTOR

        /**
         * Returns the angle created by this vector and the given
         * vector.
         * <p>
         * You can select the precision of the algorithm.
         * A high precision algorithm will provide a high precision
         * result in exchange of more computational cost.
         * A low precision algorithm will be faster, but it may provide
         * poorer results.
         * @tparam Return the result type.
         * @tparam Algorithm the algorithm configuration data.
         * @tparam OAlloc the allocator of the givem vector.
         * @param other the given vector.
         * @return the angle, casted to the result type.
         */
        template<typename Return = Type, Algorithm Algorithm = Algorithm(),
            typename OAlloc>
        Return angle(const Vec<Size, Type, OAlloc>& other);

        template<typename OAlloc>
        Vec operator+(const Vec<Size, Type, OAlloc>& other)
        const requires HasAdd<Type>;

        template<typename OAlloc>
        Vec operator-(const Vec<Size, Type, OAlloc>& other)
        const requires HasSub<Type>;

        template<typename OAlloc>
        Vec operator*(const Vec<Size, Type, OAlloc>& other)
        const requires HasMul<Type>;

        template<typename OAlloc>
        Vec operator/(const Vec<Size, Type, OAlloc>& other)
        const requires HasDiv<Type>;

        template<typename OAlloc>
        Type operator%(const Vec<Size, Type, OAlloc>& other)
        const requires HasMod<Type>;

        template<typename OAlloc>
        Vec operator<<(const Vec<Size, Type, OAlloc>& other)
        const requires HasShl<Type>;

        template<typename OAlloc>
        Vec operator>>(const Vec<Size, Type, OAlloc>& other)
        const requires HasShr<Type>;

        template<typename OAlloc>
        Vec operator&(const Vec<Size, Type, OAlloc>& other)
        const requires HasBitAnd<Type>;

        template<typename OAlloc>
        Vec operator|(const Vec<Size, Type, OAlloc>& other)
        const requires HasBitOr<Type>;

        template<typename OAlloc>
        Vec operator^(const Vec<Size, Type, OAlloc>& other)
        const requires HasBitXor<Type>;

        template<typename OAlloc>
        Vec operator&&(const Vec<Size, Type, OAlloc>& other)
        const requires HasAnd<Type>;

        template<typename OAlloc>
        Vec operator||(const Vec<Size, Type, OAlloc>& other)
        const requires HasOr<Type>;

        /**
         * Returns the dot product of this vector and the given one.
         * Both vectors must have the same data type and size.
         * @tparam OAlloc the allocator of the given vector.
         * @param other the given vector.
         * @return the result.
         */
        template<typename OAlloc>
        Type dot(const Vec<Size, Type, OAlloc>& other) const;

        /**
         * Returns the cross product of this vector and the given one.
         * Both vectors must have the same data type and a size of 3.
         * @tparam OAlloc the allocator of the given vector.
         * @param other the given vector.
         * @return the result.
         */
        template<typename OAlloc>
        Vec cross(const Vec<Size, Type, OAlloc>& other) const requires (
            Size == 3 && HasAdd<Type> && HasMul<Type>);

        template<typename OAlloc>
        bool operator==(const Vec<Size, Type, OAlloc>& other) const;

        template<typename OAlloc>
        bool operator!=(const Vec<Size, Type, OAlloc>& other) const;

        bool operator==(const VecRef<Size, Type>& other) const;

        bool operator!=(const VecRef<Size, Type>& other) const;

        // ENDREGION

        // REGION ITERATOR

        auto begin();

        auto begin() const;

        auto end();

        auto end() const;

        auto cbegin() const;

        auto cend() const;

        auto rbegin();

        auto rend();

        auto crbegin() const;

        auto crend() const;

        // ENDREGION

        // REGION GLM SUPPORT

#ifdef RUSH_GLM

        Vec(const glm::vec<Size, Type>& o) {
            for (size_t i = 0; i < Size; ++i) {
                data[i] = o[i];
            }
        }

        Vec(const glm::vec<Size, Type>&& o) {
            for (size_t i = 0; i < Size; ++i) {
                data[i] = std::move(o[i]);
            }
        }

        operator glm::vec<Size, Type>() const {
            glm::vec<Size, Type> result;
            for (size_t i = 0; i < Size; ++i) {
                result[i] = data[i];
            }
            return result;
        }

#endif

        // ENDREGION
    };
}

#include <rush/vector/vec_impl.h>

#endif //RUSH_VEC_BASE_H
