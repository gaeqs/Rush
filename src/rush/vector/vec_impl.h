//
// Created by gaelr on 06/08/2023.
//

#ifndef RUSH_VEC_IMPL_H
#define RUSH_VEC_IMPL_H

#include <bit>

namespace rush {
    template<size_t Size, typename Type, typename Allocator>
        requires (Size > 0)
    Vec<Size, Type, Allocator>::Vec() : data() {
    }


    template<size_t Size, typename Type, typename Allocator>
        requires (Size > 0)
    Vec<Size, Type, Allocator>::Vec(Type fill) {
        std::fill_n(begin(), Size, fill);
    }

    template<size_t Size, typename Type, typename Allocator>
        requires (Size > 0)
    template<typename... T>
        requires std::is_convertible_v<std::common_type_t<T...>, Type> &&
                 (Size > 1 && sizeof...(T) == Size)
    Vec<Size, Type, Allocator>::Vec(T... list) {
        auto it = begin();
        ((*it++ = list), ...);
    }

    template<size_t Size, typename Type, typename Allocator>
        requires (Size > 0)
    template<size_t OSize, typename OAlloc>
        requires(Size < OSize)
    Vec<Size, Type, Allocator>::Vec(const Vec<OSize, Type, OAlloc>& other) {
        std::copy_n(other.cbegin(), Size, begin());
    }

    template<size_t Size, typename Type, typename Allocator>
        requires (Size > 0)
    template<size_t OSize, typename OAlloc, typename... T>
        requires std::is_convertible_v<std::common_type_t<T...>, Type> &&
                 (sizeof...(T) + OSize == Size)
    Vec<Size, Type, Allocator>::Vec(const Vec<OSize, Type, OAlloc>& other,
                                    T... list) {
        auto it = begin();
        auto oIt = other.cbegin();
        for (size_t i = 0; i < OSize; ++i) {
            *it++ = *oIt++;
        }
        ((*it++ = list), ...);
    }

    template<size_t Size, typename Type, typename Allocator>
        requires (Size > 0)
    Vec<Size, Type, Allocator>::Vec(const VecRef<Size, Type>& ref) {
        for (size_t i = 0; i < Size; ++i) {
            data[i] = *ref.references[i];
        }
    }

    template<size_t Size, typename Type, typename Allocator>
        requires (Size > 0)
    Vec<Size, Type, Allocator>::Vec(std::function<Type(size_t)> populator) {
        for (size_t i = 0; i < Size; ++i) {
            data[i] = populator(i);
        }
    }

    template<size_t Size, typename Type, typename Allocator>
        requires (Size > 0)
    Vec<Size, Type, Allocator>::Vec(
        std::function<Type(size_t, size_t)> populator) {
        for (size_t i = 0; i < Size; ++i) {
            data[i] = populator(i, Size);
        }
    }

    template<size_t Size, typename Type, typename Allocator>
        requires (Size > 0)
    const Type& Vec<Size, Type, Allocator>::x() const {
        return data[0];
    }

    template<size_t Size, typename Type, typename Allocator>
        requires (Size > 0)
    Type& Vec<Size, Type, Allocator>::x() {
        return data[0];
    }

    template<size_t Size, typename Type, typename Allocator>
        requires (Size > 0)
    const Type& Vec<Size, Type, Allocator>::y() const requires (Size >= 2) {
        return data[1];
    }

    template<size_t Size, typename Type, typename Allocator>
        requires (Size > 0)
    Type& Vec<Size, Type, Allocator>::y() requires (Size >= 2) {
        return data[1];
    }

    template<size_t Size, typename Type, typename Allocator>
        requires (Size > 0)
    const Type& Vec<Size, Type, Allocator>::z() const requires (Size >= 3) {
        return data[2];
    }

    template<size_t Size, typename Type, typename Allocator>
        requires (Size > 0)
    Type& Vec<Size, Type, Allocator>::z() requires (Size >= 3) {
        return data[2];
    }

    template<size_t Size, typename Type, typename Allocator>
        requires (Size > 0)
    const Type& Vec<Size, Type, Allocator>::w() const requires (Size >= 4) {
        return data[3];
    }

    template<size_t Size, typename Type, typename Allocator>
        requires (Size > 0)
    Type& Vec<Size, Type, Allocator>::w() requires (Size >= 4) {
        return data[3];
    }

    template<size_t Size, typename Type, typename Allocator>
        requires (Size > 0)
    template<typename... Ts, typename OAlloc>
        requires std::is_convertible_v<std::common_type_t<Ts...>, size_t>
    Vec<sizeof...(Ts), Type, OAlloc>
    Vec<Size, Type, Allocator>::operator()(Ts&&... indices) const {
        Vec<sizeof...(Ts), Type, OAlloc> vec;
        size_t i = 0;
        ((vec.references[i++] = &data[indices]), ...);
        return vec;
    }

    template<size_t Size, typename Type, typename Allocator>
        requires (Size > 0)
    template<typename... Ts>
        requires std::is_convertible_v<std::common_type_t<Ts...>, size_t>
    VecRef<sizeof...(Ts), Type>
    Vec<Size, Type, Allocator>::operator()(Ts&&... indices) {
        VecRef<sizeof...(Ts), Type> vec;
        size_t i = 0;
        ((vec.references[i++] = &data[indices]), ...);
        return vec;
    }

    template<size_t Size, typename Type, typename Allocator>
        requires (Size > 0)
    Type& Vec<Size, Type, Allocator>::operator[](size_t index) {
        return data[index];
    }

    template<size_t Size, typename Type, typename Allocator>
        requires (Size > 0)
    const Type&
    Vec<Size, Type, Allocator>::operator[](size_t index) const {
        return data[index];
    }

    template<size_t Size, typename Type, typename Allocator>
        requires (Size > 0)
    const Type* Vec<Size, Type, Allocator>::toPointer() const {
        return data.toPointer();
    }

    template<size_t Size, typename Type, typename Allocator>
        requires (Size > 0)
    Type* Vec<Size, Type, Allocator>::toPointer() {
        return data.toPointer();
    }

    template<size_t Size, typename Type, typename Allocator>
        requires (Size > 0)
    constexpr size_t Vec<Size, Type, Allocator>::size() const {
        return Size;
    }

    template<size_t Size, typename Type, typename Allocator>
        requires (Size > 0)
    Type Vec<Size, Type, Allocator>::squaredLength() const {
        return this->dot(*this);
    }

    template<size_t Size, typename Type, typename Allocator>
        requires (Size > 0)
    template<typename Return>
    Return Vec<Size, Type, Allocator>::length() const requires (
        std::is_convertible_v<Type, Return> && HasSquaredRoot<Type>) {
        return static_cast<Return>(std::sqrt(squaredLength()));
    }

    template<size_t Size, typename Type, typename Allocator>
        requires (Size > 0)
    template<typename Return, Algorithm A>
    Return Vec<Size, Type, Allocator>::inverseLength() const requires (
        std::is_convertible_v<Type, Return> && HasSquaredRoot<Type>) {
        if constexpr (A.precision == Precision::High) {
            return 1.0f / std::sqrt(squaredLength());
        }

        if constexpr (std::is_same_v<Return, float>) {
            auto y = static_cast<float>(squaredLength());

#ifdef RUSH_INTRINSICS
            if constexpr (A.useIntrinsics()) {
                return _mm_cvtss_f32(_mm_rsqrt_ss(_mm_set_ss(y)));
            }
#endif

            float x2 = y * 0.5f;
            // what the fuck?
            uint32_t i = 0x5f3759df - (std::bit_cast<uint32_t>(y) >> 1);
            y = std::bit_cast<float>(i);
            return y * (1.5f - x2 * y * y);
        }
        else if constexpr (std::is_same_v<Return, double>) {
            auto y = static_cast<double>(squaredLength());
            double x2 = y * 0.5f;
            // what the fuck?
            uint64_t i = 0x5fe6eb50c7b537a9 - (std::bit_cast<uint64_t>(y) >> 1);
            y = std::bit_cast<double>(i);
            return y * (1.5f - x2 * y * y);
        }

        return 1.0f / std::sqrt(squaredLength());
    }

    template<size_t Size, typename Type, typename Allocator>
        requires (Size > 0)
    template<typename Return, Algorithm A, typename OAlloc>
    Vec<Size, Return, OAlloc>
    Vec<Size, Type, Allocator>::normalized() const requires HasMul<Return> {
        Return invLen = inverseLength<Return, A>();
        Vec<Size, Return, OAlloc> result;
        for (size_t i = 0; i < Size; ++i) {
            result.data[i] = static_cast<Return>(data[i]) * invLen;
        }
        return result;
    }

    template<size_t Size, typename Type, typename Allocator>
        requires (Size > 0)
    template<typename To, typename OAlloc>
    Vec<Size, To, OAlloc> Vec<Size, Type, Allocator>::cast() const {
        return Vec<Size, To, OAlloc>([this](size_t i) {
            return static_cast<To>(data[i]);
        });
    }

    template<size_t Size, typename Type, typename Allocator> requires (Size > 0)
    template<typename OAlloc>
    Vec<Size, Type, OAlloc> Vec<Size, Type, Allocator>::reverse() const {
        return Vec<Size, Type, OAlloc>([this](size_t i) {
            return data[Size - i];
        });
    }

    template<size_t Size, typename Type, typename Allocator>
        requires (Size > 0)
    std::array<Type, Size> Vec<Size, Type, Allocator>::toArray() const {
        std::array<Type, Size> array;
        for (int i = 0; i < Size; ++i) {
            array[i] = data[i];
        }
        return array;
    }

    template<size_t Size, typename Type, typename Allocator>
        requires (Size > 0)
    Vec<Size, Type, Allocator>&
    Vec<Size, Type, Allocator>::operator+() {
        return *this;
    }

    template<size_t Size, typename Type, typename Allocator>
        requires (Size > 0)
    const Vec<Size, Type, Allocator>&
    Vec<Size, Type, Allocator>::operator+() const {
        return *this;
    }

    template<size_t Size, typename Type, typename Allocator>
        requires (Size > 0)
    Vec<Size, Type, Allocator>
    Vec<Size, Type, Allocator>::operator-() const requires HasSub<Type> {
        Vec result;
        for (size_t i = 0; i < Size; ++i) {
            result[i] = -data[i];
        }
        return result;
    }

    template<size_t Size, typename Type, typename Allocator>
        requires (Size > 0)
    Vec<Size, Type, Allocator>&
    Vec<Size, Type, Allocator>::operator+=(
        const Type& s) requires HasAdd<Type> {
        for (size_t i = 0; i < Size; ++i) {
            data[i] += s;
        }
        return *this;
    }

    template<size_t Size, typename Type, typename Allocator>
        requires (Size > 0)
    Vec<Size, Type, Allocator>&
    Vec<Size, Type, Allocator>::operator-=(
        const Type& s) requires HasSub<Type> {
        for (size_t i = 0; i < Size; ++i) {
            data[i] -= s;
        }
        return *this;
    }

    template<size_t Size, typename Type, typename Allocator>
        requires (Size > 0)
    Vec<Size, Type, Allocator>&
    Vec<Size, Type, Allocator>::operator*=(
        const Type& s) requires HasMul<Type> {
        for (size_t i = 0; i < Size; ++i) {
            data[i] *= s;
        }
        return *this;
    }

    template<size_t Size, typename Type, typename Allocator>
        requires (Size > 0)
    Vec<Size, Type, Allocator>&
    Vec<Size, Type, Allocator>::operator/=(
        const Type& s) requires HasDiv<Type> {
        for (size_t i = 0; i < Size; ++i) {
            data[i] /= s;
        }
        return *this;
    }

    template<size_t Size, typename Type, typename Allocator>
        requires (Size > 0)
    Vec<Size, Type, Allocator>&
    Vec<Size, Type, Allocator>::operator<<=(
        const Type& s) requires HasShl<Type> {
        for (size_t i = 0; i < Size; ++i) {
            data[i] <<= s;
        }
        return *this;
    }

    template<size_t Size, typename Type, typename Allocator>
        requires (Size > 0)
    Vec<Size, Type, Allocator>&
    Vec<Size, Type, Allocator>::operator>>=(
        const Type& s) requires HasShr<Type> {
        for (size_t i = 0; i < Size; ++i) {
            data[i] >>= s;
        }
        return *this;
    }

    template<size_t Size, typename Type, typename Allocator>
        requires (Size > 0)
    Vec<Size, Type, Allocator>&
    Vec<Size, Type, Allocator>::operator&=(
        const Type& s) requires HasBitAnd<Type> {
        for (size_t i = 0; i < Size; ++i) {
            data[i] &= s;
        }
        return *this;
    }

    template<size_t Size, typename Type, typename Allocator>
        requires (Size > 0)
    Vec<Size, Type, Allocator>&
    Vec<Size, Type, Allocator>::operator|=(
        const Type& s) requires HasBitOr<Type> {
        for (size_t i = 0; i < Size; ++i) {
            data[i] |= s;
        }
        return *this;
    }

    template<size_t Size, typename Type, typename Allocator>
        requires (Size > 0)
    Vec<Size, Type, Allocator>&
    Vec<Size, Type, Allocator>::operator^=(
        const Type& s) requires HasBitXor<Type> {
        for (size_t i = 0; i < Size; ++i) {
            data[i] ^= s;
        }
        return *this;
    }

    template<size_t Size, typename Type, typename Allocator>
        requires (Size > 0)
    template<typename OAlloc>
    Vec<Size, Type, Allocator>&
    Vec<Size, Type, Allocator>::operator+=(
        const Vec<Size, Type, OAlloc>& o) requires HasAdd<Type> {
        for (size_t i = 0; i < Size; ++i) {
            data[i] += o[i];
        }
        return *this;
    }

    template<size_t Size, typename Type, typename Allocator>
        requires (Size > 0)
    template<typename OAlloc>
    Vec<Size, Type, Allocator>&
    Vec<Size, Type, Allocator>::operator-=(
        const Vec<Size, Type, OAlloc>& o) requires HasSub<Type> {
        for (size_t i = 0; i < Size; ++i) {
            data[i] -= o[i];
        }
        return *this;
    }

    template<size_t Size, typename Type, typename Allocator>
        requires (Size > 0)
    template<typename OAlloc>
    Vec<Size, Type, Allocator>&
    Vec<Size, Type, Allocator>::operator*=(
        const Vec<Size, Type, OAlloc>& o) requires HasMul<Type> {
        for (size_t i = 0; i < Size; ++i) {
            data[i] *= o[i];
        }
        return *this;
    }

    template<size_t Size, typename Type, typename Allocator>
        requires (Size > 0)
    template<typename OAlloc>
    Vec<Size, Type, Allocator>&
    Vec<Size, Type, Allocator>::operator/=(
        const Vec<Size, Type, OAlloc>& o) requires HasDiv<Type> {
        for (size_t i = 0; i < Size; ++i) {
            data[i] /= o[i];
        }
        return *this;
    }

    template<size_t Size, typename Type, typename Allocator>
        requires (Size > 0)
    template<typename OAlloc>
    Vec<Size, Type, Allocator>&
    Vec<Size, Type, Allocator>::operator<<=(
        const Vec<Size, Type, OAlloc>& o) requires HasShl<Type> {
        for (size_t i = 0; i < Size; ++i) {
            data[i] <<= o[i];
        }
        return *this;
    }

    template<size_t Size, typename Type, typename Allocator>
        requires (Size > 0)
    template<typename OAlloc>
    Vec<Size, Type, Allocator>&
    Vec<Size, Type, Allocator>::operator>>=(
        const Vec<Size, Type, OAlloc>& o) requires HasShr<Type> {
        for (size_t i = 0; i < Size; ++i) {
            data[i] >>= o[i];
        }
        return *this;
    }

    template<size_t Size, typename Type, typename Allocator>
        requires (Size > 0)
    template<typename OAlloc>
    Vec<Size, Type, Allocator>&
    Vec<Size, Type, Allocator>::operator&=(
        const Vec<Size, Type, OAlloc>& o) requires HasBitAnd<Type> {
        for (size_t i = 0; i < Size; ++i) {
            data[i] &= o[i];
        }
        return *this;
    }

    template<size_t Size, typename Type, typename Allocator>
        requires (Size > 0)
    template<typename OAlloc>
    Vec<Size, Type, Allocator>&
    Vec<Size, Type, Allocator>::operator|=(
        const Vec<Size, Type, OAlloc>& o) requires HasBitOr<Type> {
        for (size_t i = 0; i < Size; ++i) {
            data[i] |= o[i];
        }
        return *this;
    }

    template<size_t Size, typename Type, typename Allocator>
        requires (Size > 0)
    template<typename OAlloc>
    Vec<Size, Type, Allocator>&
    Vec<Size, Type, Allocator>::operator^=(
        const Vec<Size, Type, OAlloc>& o) requires HasBitXor<Type> {
        for (size_t i = 0; i < Size; ++i) {
            data[i] ^= o[i];
        }
        return *this;
    }

    template<size_t Size, typename Type, typename Allocator>
        requires (Size > 0)
    Vec<Size, Type, Allocator>
    Vec<Size, Type, Allocator>::operator+(
        const Type& s) const requires HasAdd<Type> {
        Vec result;
        for (size_t i = 0; i < Size; ++i) {
            result[i] = data[i] + s;
        }
        return result;
    }

    template<size_t Size, typename Type, typename Allocator>
        requires (Size > 0)
    Vec<Size, Type, Allocator>
    Vec<Size, Type, Allocator>::operator-(
        const Type& s) const requires HasSub<Type> {
        Vec result;
        for (size_t i = 0; i < Size; ++i) {
            result[i] = data[i] - s;
        }
        return result;
    }

    template<size_t Size, typename Type, typename Allocator>
        requires (Size > 0)
    Vec<Size, Type, Allocator>
    Vec<Size, Type, Allocator>::operator*(
        const Type& s) const requires HasMul<Type> {
        Vec result;
        for (size_t i = 0; i < Size; ++i) {
            result[i] = data[i] * s;
        }
        return result;
    }

    template<size_t Size, typename Type, typename Allocator>
        requires (Size > 0)
    Vec<Size, Type, Allocator>
    Vec<Size, Type, Allocator>::operator/(
        const Type& s) const requires HasDiv<Type> {
        Vec result;
        for (size_t i = 0; i < Size; ++i) {
            result[i] = data[i] / s;
        }
        return result;
    }

    template<size_t Size, typename Type, typename Allocator>
        requires (Size > 0)
    Vec<Size, Type, Allocator>
    Vec<Size, Type, Allocator>::operator%(
        const Type& s) const requires HasMod<Type> {
        Vec result;
        for (size_t i = 0; i < Size; ++i) {
            result[i] = data[i] % s;
        }
        return result;
    }

    template<size_t Size, typename Type, typename Allocator>
        requires (Size > 0)
    Vec<Size, Type, Allocator>
    Vec<Size, Type, Allocator>::operator<<(
        const Type& s) const requires HasShl<Type> {
        Vec result;
        for (size_t i = 0; i < Size; ++i) {
            result[i] = data[i] << s;
        }
        return result;
    }

    template<size_t Size, typename Type, typename Allocator>
        requires (Size > 0)
    Vec<Size, Type, Allocator>
    Vec<Size, Type, Allocator>::operator>>(
        const Type& s) const requires HasShr<Type> {
        Vec result;
        for (size_t i = 0; i < Size; ++i) {
            result[i] = data[i] >> s;
        }
        return result;
    }

    template<size_t Size, typename Type, typename Allocator>
        requires (Size > 0)
    Vec<Size, Type, Allocator>
    Vec<Size, Type, Allocator>::operator&(
        const Type& s) const requires HasBitAnd<Type> {
        Vec result;
        for (size_t i = 0; i < Size; ++i) {
            result[i] = data[i] & s;
        }
        return result;
    }

    template<size_t Size, typename Type, typename Allocator>
        requires (Size > 0)
    Vec<Size, Type, Allocator>
    Vec<Size, Type, Allocator>::operator|(
        const Type& s) const requires HasBitOr<Type> {
        Vec result;
        for (size_t i = 0; i < Size; ++i) {
            result[i] = data[i] | s;
        }
        return result;
    }

    template<size_t Size, typename Type, typename Allocator>
        requires (Size > 0)
    Vec<Size, Type, Allocator>
    Vec<Size, Type, Allocator>::operator^(
        const Type& s) const requires HasBitXor<Type> {
        Vec result;
        for (size_t i = 0; i < Size; ++i) {
            result[i] = data[i] ^ s;
        }
        return result;
    }

    template<size_t Size, typename Type, typename Allocator>
        requires (Size > 0)
    Vec<Size, Type, Allocator>
    Vec<Size, Type, Allocator>::operator&&(
        const Type& s) const requires HasAnd<Type> {
        Vec result;
        for (size_t i = 0; i < Size; ++i) {
            result[i] = data[i] && s;
        }
        return result;
    }

    template<size_t Size, typename Type, typename Allocator>
        requires (Size > 0)
    Vec<Size, Type, Allocator>
    Vec<Size, Type, Allocator>::operator||(
        const Type& s) const requires HasOr<Type> {
        Vec result;
        for (size_t i = 0; i < Size; ++i) {
            result[i] = data[i] || s;
        }
        return result;
    }

    template<size_t Size, typename Type, typename Allocator>
        requires (Size > 0)
    template<typename Return, Algorithm A, typename OAlloc>
    Return
    Vec<Size, Type, Allocator>::angle(const Vec<Size, Type, OAlloc>& other) {
        if constexpr (A.precision == Precision::High && Size == 3) {
            // Use atan2!
            Return m = Return(1.0) / std::sqrt(squaredLength()
                                               * other.squaredLength());
            Return c = dot(other) * m;
            Return s = cross(other).length() * m;
            return std::atan2(s, c);
        }

        Return inv = std::sqrt(squaredLength() * other.squaredLength());
        return std::acos(dot(other) / inv);
    }

    template<size_t Size, typename Type, typename Allocator>
        requires (Size > 0)
    template<typename OAlloc>
    Vec<Size, Type, Allocator>
    Vec<Size, Type, Allocator>::operator+(
        const Vec<Size, Type, OAlloc>& other) const requires
        HasAdd<Type> {
        Vec result;
        for (size_t i = 0; i < Size; ++i) {
            result[i] = data[i] + other[i];
        }
        return result;
    }

    template<size_t Size, typename Type, typename Allocator>
        requires (Size > 0)
    template<typename OAlloc>
    Vec<Size, Type, Allocator>
    Vec<Size, Type, Allocator>::operator-(
        const Vec<Size, Type, OAlloc>& other) const requires
        HasSub<Type> {
        Vec result;
        for (size_t i = 0; i < Size; ++i) {
            result[i] = data[i] - other[i];
        }
        return result;
    }

    template<size_t Size, typename Type, typename Allocator>
        requires (Size > 0)
    template<typename OAlloc>
    Vec<Size, Type, Allocator>
    Vec<Size, Type, Allocator>::operator*(
        const Vec<Size, Type, OAlloc>& other) const requires
        HasMul<Type> {
        Vec result;
        for (size_t i = 0; i < Size; ++i) {
            result[i] = data[i] * other[i];
        }
        return result;
    }

    template<size_t Size, typename Type, typename Allocator>
        requires (Size > 0)
    template<typename OAlloc>
    Vec<Size, Type, Allocator>
    Vec<Size, Type, Allocator>::operator/(
        const Vec<Size, Type, OAlloc>& other) const requires
        HasDiv<Type> {
        Vec result;
        for (size_t i = 0; i < Size; ++i) {
            result[i] = data[i] / other[i];
        }
        return result;
    }

    template<size_t Size, typename Type, typename Allocator>
        requires (Size > 0)
    template<typename OAlloc>
    Type Vec<Size, Type, Allocator>::operator%(const Vec<Size, Type,
        OAlloc>& other) const requires HasMod<Type> {
        Vec result;
        for (size_t i = 0; i < Size; ++i) {
            result[i] = data[i] % other[i];
        }
        return result;
    }

    template<size_t Size, typename Type, typename Allocator>
        requires (Size > 0)
    template<typename OAlloc>
    Vec<Size, Type, Allocator>
    Vec<Size, Type, Allocator>::operator<<(
        const Vec<Size, Type, OAlloc>& other) const requires
        HasShl<Type> {
        Vec result;
        for (size_t i = 0; i < Size; ++i) {
            result[i] = data[i] << other[i];
        }
        return result;
    }

    template<size_t Size, typename Type, typename Allocator>
        requires (Size > 0)
    template<typename OAlloc>
    Vec<Size, Type, Allocator>
    Vec<Size, Type, Allocator>::operator>>(
        const Vec<Size, Type, OAlloc>& other) const requires HasShr<Type> {
        Vec result;
        for (size_t i = 0; i < Size; ++i) {
            result[i] = data[i] >> other[i];
        }
        return result;
    }

    template<size_t Size, typename Type, typename Allocator>
        requires (Size > 0)
    template<typename OAlloc>
    Vec<Size, Type, Allocator>
    Vec<Size, Type, Allocator>::operator&(
        const Vec<Size, Type, OAlloc>& other)
    const requires HasBitAnd<Type> {
        Vec result;
        for (size_t i = 0; i < Size; ++i) {
            result[i] = data[i] & other[i];
        }
        return result;
    }

    template<size_t Size, typename Type, typename Allocator>
        requires (Size > 0)
    template<typename OAlloc>
    Vec<Size, Type, Allocator>
    Vec<Size, Type, Allocator>::operator|(
        const Vec<Size, Type, OAlloc>& other)
    const requires HasBitOr<Type> {
        Vec result;
        for (size_t i = 0; i < Size; ++i) {
            result[i] = data[i] | other[i];
        }
        return result;
    }

    template<size_t Size, typename Type, typename Allocator>
        requires (Size > 0)
    template<typename OAlloc>
    Vec<Size, Type, Allocator>
    Vec<Size, Type, Allocator>::operator^(
        const Vec<Size, Type, OAlloc>& other)
    const requires HasBitXor<Type> {
        Vec result;
        for (size_t i = 0; i < Size; ++i) {
            result[i] = data[i] ^ other[i];
        }
        return result;
    }

    template<size_t Size, typename Type, typename Allocator>
        requires (Size > 0)
    template<typename OAlloc>
    Vec<Size, Type, Allocator>
    Vec<Size, Type, Allocator>::operator&&(
        const Vec<Size, Type, OAlloc>& other) const requires
        HasAnd<Type> {
        Vec result;
        for (size_t i = 0; i < Size; ++i) {
            result[i] = data[i] && other[i];
        }
        return result;
    }

    template<size_t Size, typename Type, typename Allocator>
        requires (Size > 0)
    template<typename OAlloc>
    Vec<Size, Type, Allocator>
    Vec<Size, Type, Allocator>::operator||(
        const Vec<Size, Type, OAlloc>& other) const requires
        HasOr<Type> {
        Vec result;
        for (size_t i = 0; i < Size; ++i) {
            result[i] = data[i] || other[i];
        }
        return result;
    }

    template<size_t Size, typename Type, typename Allocator>
        requires (Size > 0)
    template<typename OAlloc>
    Type Vec<Size, Type, Allocator>::dot(
        const Vec<Size, Type, OAlloc>& other) const {
        Type result = data[0] * other[0];
        for (size_t i = 1; i < Size; ++i) {
            result += data[i] * other[i];
        }
        return result;
    }

    template<size_t Size, typename Type, typename Allocator>
        requires (Size > 0)
    template<typename OAlloc>
    Vec<Size, Type, Allocator>
    Vec<Size, Type, Allocator>::cross(
        const Vec<Size, Type, OAlloc>& other) const requires (
        Size == 3 && HasAdd<Type> && HasMul<Type>) {
        return {
            y() * other.z() - other.y() * z(),
            z() * other.x() - other.z() * x(),
            x() * other.y() - other.x() * y()
        };
    }

    template<size_t Size, typename Type, typename Allocator>
        requires (Size > 0)
    template<typename OAlloc>
    bool Vec<Size, Type, Allocator>::operator==(
        const Vec<Size, Type, OAlloc>& other) const {
        if constexpr (std::is_same_v<Vec, Vec<Size, Type, OAlloc>>) {
            if (this == &other) return true;
        }
        return std::equal(cbegin(), cend(), other.cbegin());
    }

    template<size_t Size, typename Type, typename Allocator>
        requires (Size > 0)
    template<typename OAlloc>
    bool Vec<Size, Type, Allocator>::operator!=(
        const Vec<Size, Type, OAlloc>& other) const {
        if constexpr (std::is_same_v<Vec, Vec<Size, Type, OAlloc>>) {
            if (this == &other) return false;
        }
        return !std::equal(cbegin(), cend(), other.cbegin());
    }

    template<size_t Size, typename Type, typename Allocator>
        requires (Size > 0)
    bool Vec<Size, Type, Allocator>::operator==(
        const VecRef<Size, Type>& other) const {
        for (int i = 0; i < Size; ++i) {
            if (data[i] != *other.references[i]) return false;
        }
        return true;
    }

    template<size_t Size, typename Type, typename Allocator>
        requires (Size > 0)
    bool Vec<Size, Type, Allocator>::operator!=(
        const VecRef<Size, Type>& other) const {
        for (int i = 0; i < Size; ++i) {
            if (data[i] != *other.references[i]) return true;
        }
        return false;
    }

    template<size_t Size, typename Type, typename Allocator>
        requires (Size > 0)
    auto Vec<Size, Type, Allocator>::begin() {
        return data.begin();
    }

    template<size_t Size, typename Type, typename Allocator>
        requires (Size > 0)
    auto Vec<Size, Type, Allocator>::begin() const {
        return data.cbegin();
    }

    template<size_t Size, typename Type, typename Allocator>
        requires (Size > 0)
    auto Vec<Size, Type, Allocator>::end() {
        return data.end();
    }

    template<size_t Size, typename Type, typename Allocator>
        requires (Size > 0)
    auto Vec<Size, Type, Allocator>::end() const {
        return data.cend();
    }

    template<size_t Size, typename Type, typename Allocator>
        requires (Size > 0)
    auto Vec<Size, Type, Allocator>::cbegin() const {
        return data.cbegin();
    }

    template<size_t Size, typename Type, typename Allocator>
        requires (Size > 0)
    auto Vec<Size, Type, Allocator>::cend() const {
        return data.cend();
    }

    template<size_t Size, typename Type, typename Allocator>
        requires (Size > 0)
    auto Vec<Size, Type, Allocator>::rbegin() {
        return data.rbegin();
    }

    template<size_t Size, typename Type, typename Allocator>
        requires (Size > 0)
    auto Vec<Size, Type, Allocator>::rend() {
        return data.rend();
    }

    template<size_t Size, typename Type, typename Allocator>
        requires (Size > 0)
    auto Vec<Size, Type, Allocator>::crbegin() const {
        return data.crbegin();
    }

    template<size_t Size, typename Type, typename Allocator>
        requires (Size > 0)
    auto Vec<Size, Type, Allocator>::crend() const {
        return data.crend();
    }
}
#endif //RUSH_VEC_IMPL_H
