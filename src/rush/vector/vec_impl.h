//
// Created by gaelr on 06/08/2023.
//

#ifndef RUSH_VEC_IMPL_H
#define RUSH_VEC_IMPL_H

#include <bit>

namespace rush {

    template<size_t Size, typename Type>
    requires (Size > 0)
    template<typename... T>
    requires std::is_convertible_v<std::common_type_t<T...>, Type> &&
             (sizeof...(T) <= Size)
    Vec<Size, Type>::Vec(T... list) : data{list...} {
    }

    template<size_t Size, typename Type>
    requires (Size > 0)
    Vec<Size, Type>::Vec() : data() {
    }

    template<size_t Size, typename Type>
    requires (Size > 0)
    template<size_t OSize>
    requires(Size < OSize)
    Vec<Size, Type>::Vec(const Vec<OSize, Type>& other) {
        std::copy_n(other.toPointer(), Size, data.data());
    }

    template<size_t Size, typename Type>
    requires (Size > 0)
    Vec<Size, Type>::Vec(const VecRef<Size, Type>& ref) {
        for (size_t i = 0; i < Size; ++i) {
            data[i] = *ref.references[i];
        }
    }

    template<size_t Size, typename Type>
    requires (Size > 0)
    Vec<Size, Type>::Vec(std::function<Type(size_t)> populator) {
        for (size_t i = 0; i < Size; ++i) {
            data[i] = populator(i);
        }
    }

    template<size_t Size, typename Type>
    requires (Size > 0)
    Vec<Size, Type>::Vec(std::function<Type(size_t, size_t)> populator) {
        for (size_t i = 0; i < Size; ++i) {
            data[i] = populator(i, Size);
        }
    }

    template<size_t Size, typename Type>
    requires (Size > 0)
    const Type& Vec<Size, Type>::x() const {
        return data[0];
    }

    template<size_t Size, typename Type>
    requires (Size > 0)
    Type& Vec<Size, Type>::x() {
        return data[0];
    }

    template<size_t Size, typename Type>
    requires (Size > 0)
    const Type& Vec<Size, Type>::y() const requires (Size >= 2) {
        return data[1];
    }

    template<size_t Size, typename Type>
    requires (Size > 0)
    Type& Vec<Size, Type>::y() requires (Size >= 2) {
        return data[1];
    }

    template<size_t Size, typename Type>
    requires (Size > 0)
    const Type& Vec<Size, Type>::z() const requires (Size >= 3) {
        return data[2];
    }

    template<size_t Size, typename Type>
    requires (Size > 0)
    Type& Vec<Size, Type>::z() requires (Size >= 3) {
        return data[2];
    }

    template<size_t Size, typename Type>
    requires (Size > 0)
    const Type& Vec<Size, Type>::w() const requires (Size >= 4) {
        return data[3];
    }

    template<size_t Size, typename Type>
    requires (Size > 0)
    Type& Vec<Size, Type>::w() requires (Size >= 4) {
        return data[3];
    }

    template<size_t Size, typename Type>
    requires (Size > 0)
    template<typename... Ts>
    requires std::is_convertible_v<std::common_type_t<Ts...>, size_t>
    Vec<sizeof...(Ts), Type>
    Vec<Size, Type>::operator()(Ts&& ... indices) const {
        Vec<sizeof...(Ts), Type> vec;
        size_t i = 0;
        ((vec.references[i++] = &(*this)[indices]), ...);
        return vec;
    }

    template<size_t Size, typename Type>
    requires (Size > 0)
    template<typename... Ts>
    requires std::is_convertible_v<std::common_type_t<Ts...>, size_t>
    VecRef<sizeof...(Ts), Type> Vec<Size, Type>::operator()(Ts&& ... indices) {
        VecRef<sizeof...(Ts), Type> vec;
        size_t i = 0;
        ((vec.references[i++] = &(*this)[indices]), ...);
        return vec;
    }

    template<size_t Size, typename Type>
    requires (Size > 0)
    Type& Vec<Size, Type>::operator[](size_t index) {
        return data[index];
    }

    template<size_t Size, typename Type>
    requires (Size > 0)
    const Type&
    Vec<Size, Type>::operator[](size_t index) const {
        return data[index];
    }

    template<size_t Size, typename Type>
    requires (Size > 0)
    const Type* Vec<Size, Type>::toPointer() const {
        return data.data();
    }

    template<size_t Size, typename Type>
    requires (Size > 0)
    Type* Vec<Size, Type>::toPointer() {
        return data.data();
    }

    template<size_t Size, typename Type>
    requires (Size > 0)
    constexpr size_t Vec<Size, Type>::size() const {
        return Size;
    }

    template<size_t Size, typename Type>
    requires (Size > 0)
    Type Vec<Size, Type>::squaredLength() const {
        return this->dot(*this);
    }

    template<size_t Size, typename Type>
    requires (Size > 0)
    template<typename Return>
    Return Vec<Size, Type>::length() const requires (
    std::is_convertible_v<Type, Return> && HasSquaredRoot<Type>) {
        return std::sqrt(squaredLength());
    }

    template<size_t Size, typename Type>
    requires (Size > 0)
    template<typename Return, Algorithm A>
    Return Vec<Size, Type>::inverseLength() const requires (
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
        } else if constexpr (std::is_same_v<Return, double>) {
            auto y = static_cast<double >(squaredLength());
            double x2 = y * 0.5f;
            // what the fuck?
            uint64_t i = 0x5fe6eb50c7b537a9 - (std::bit_cast<uint64_t>(y) >> 1);
            y = std::bit_cast<double>(i);
            return y * (1.5f - x2 * y * y);
        }

        return 1.0f / std::sqrt(squaredLength());
    }

    template<size_t Size, typename Type>
    requires (Size > 0)
    template<typename Return, Algorithm A>
    Vec<Size, Return>
    Vec<Size, Type>::normalized() const requires HasMul<Return> {
        Return invLen = inverseLength<Return, A>();
        Vec<Size, Return> result;
        for (size_t i = 0; i < Size; ++i) {
            result.data[i] = static_cast<Return>(data[i]) * invLen;
        }
        return result;
    }

    template<size_t Size, typename Type>
    requires (Size > 0)
    typename Vec<Size, Type>::Self& Vec<Size, Type>::operator+() {
        return *this;
    }

    template<size_t Size, typename Type>
    requires (Size > 0)
    const typename Vec<Size, Type>::Self& Vec<Size, Type>::operator+() const {
        return *this;
    }

    template<size_t Size, typename Type>
    requires (Size > 0)
    Vec<Size, Type>::Self
    Vec<Size, Type>::operator-() const requires HasSub<Type> {
        Self result;
        for (size_t i = 0; i < Size; ++i) {
            result[i] = -data[i];
        }
        return result;
    }

    template<size_t Size, typename Type>
    requires (Size > 0)
    typename Vec<Size, Type>::Self&
    Vec<Size, Type>::operator+=(const Type& s) requires HasAdd<Type> {
        for (size_t i = 0; i < Size; ++i) {
            data[i] += s;
        }
        return *this;
    }

    template<size_t Size, typename Type>
    requires (Size > 0)
    typename Vec<Size, Type>::Self&
    Vec<Size, Type>::operator-=(const Type& s) requires HasSub<Type> {
        for (size_t i = 0; i < Size; ++i) {
            data[i] -= s;
        }
        return *this;
    }

    template<size_t Size, typename Type>
    requires (Size > 0)
    typename Vec<Size, Type>::Self&
    Vec<Size, Type>::operator*=(const Type& s) requires HasMul<Type> {
        for (size_t i = 0; i < Size; ++i) {
            data[i] *= s;
        }
        return *this;
    }

    template<size_t Size, typename Type>
    requires (Size > 0)
    typename Vec<Size, Type>::Self&
    Vec<Size, Type>::operator/=(const Type& s) requires HasDiv<Type> {
        for (size_t i = 0; i < Size; ++i) {
            data[i] /= s;
        }
        return *this;
    }

    template<size_t Size, typename Type>
    requires (Size > 0)
    typename Vec<Size, Type>::Self&
    Vec<Size, Type>::operator<<=(const Type& s) requires HasShl<Type> {
        for (size_t i = 0; i < Size; ++i) {
            data[i] <<= s;
        }
        return *this;
    }

    template<size_t Size, typename Type>
    requires (Size > 0)
    typename Vec<Size, Type>::Self&
    Vec<Size, Type>::operator>>=(const Type& s) requires HasShr<Type> {
        for (size_t i = 0; i < Size; ++i) {
            data[i] >>= s;
        }
        return *this;
    }

    template<size_t Size, typename Type>
    requires (Size > 0)
    typename Vec<Size, Type>::Self&
    Vec<Size, Type>::operator&=(const Type& s) requires HasBitAnd<Type> {
        for (size_t i = 0; i < Size; ++i) {
            data[i] &= s;
        }
        return *this;
    }

    template<size_t Size, typename Type>
    requires (Size > 0)
    typename Vec<Size, Type>::Self&
    Vec<Size, Type>::operator|=(const Type& s) requires HasBitOr<Type> {
        for (size_t i = 0; i < Size; ++i) {
            data[i] |= s;
        }
        return *this;
    }

    template<size_t Size, typename Type>
    requires (Size > 0)
    typename Vec<Size, Type>::Self&
    Vec<Size, Type>::operator^=(const Type& s) requires HasBitXor<Type> {
        for (size_t i = 0; i < Size; ++i) {
            data[i] ^= s;
        }
        return *this;
    }

    template<size_t Size, typename Type>
    requires (Size > 0)
    typename Vec<Size, Type>::Self&
    Vec<Size, Type>::operator+=(const Self& o) requires HasAdd<Type> {
        for (size_t i = 0; i < Size; ++i) {
            data[i] += o[i];
        }
        return *this;
    }

    template<size_t Size, typename Type>
    requires (Size > 0)
    typename Vec<Size, Type>::Self&
    Vec<Size, Type>::operator-=(const Self& o) requires HasSub<Type> {
        for (size_t i = 0; i < Size; ++i) {
            data[i] -= o[i];
        }
        return *this;
    }

    template<size_t Size, typename Type>
    requires (Size > 0)
    typename Vec<Size, Type>::Self&
    Vec<Size, Type>::operator*=(const Self& o) requires HasMul<Type> {
        for (size_t i = 0; i < Size; ++i) {
            data[i] *= o[i];
        }
        return *this;
    }

    template<size_t Size, typename Type>
    requires (Size > 0)
    typename Vec<Size, Type>::Self&
    Vec<Size, Type>::operator/=(const Self& o) requires HasDiv<Type> {
        for (size_t i = 0; i < Size; ++i) {
            data[i] /= o[i];
        }
        return *this;
    }

    template<size_t Size, typename Type>
    requires (Size > 0)
    typename Vec<Size, Type>::Self&
    Vec<Size, Type>::operator<<=(const Self& o) requires HasShl<Type> {
        for (size_t i = 0; i < Size; ++i) {
            data[i] <<= o[i];
        }
        return *this;
    }

    template<size_t Size, typename Type>
    requires (Size > 0)
    typename Vec<Size, Type>::Self&
    Vec<Size, Type>::operator>>=(const Self& o) requires HasShr<Type> {
        for (size_t i = 0; i < Size; ++i) {
            data[i] >>= o[i];
        }
        return *this;
    }

    template<size_t Size, typename Type>
    requires (Size > 0)typename Vec<Size, Type>::Self&
    Vec<Size, Type>::operator&=(const Self& o) requires HasBitAnd<Type> {
        for (size_t i = 0; i < Size; ++i) {
            data[i] &= o[i];
        }
        return *this;
    }

    template<size_t Size, typename Type>
    requires (Size > 0)
    typename Vec<Size, Type>::Self&
    Vec<Size, Type>::operator|=(const Self& o) requires HasBitOr<Type> {
        for (size_t i = 0; i < Size; ++i) {
            data[i] |= o[i];
        }
        return *this;
    }

    template<size_t Size, typename Type>
    requires (Size > 0)
    typename Vec<Size, Type>::Self&
    Vec<Size, Type>::operator^=(const Self& o) requires HasBitXor<Type> {
        for (size_t i = 0; i < Size; ++i) {
            data[i] ^= o[i];
        }
        return *this;
    }

    template<size_t Size, typename Type>
    requires (Size > 0)
    Vec<Size, Type>::Self
    Vec<Size, Type>::operator+(const Type& s) const requires HasAdd<Type> {
        Self result;
        for (size_t i = 0; i < Size; ++i) {
            result[i] = data[i] + s;
        }
        return result;
    }

    template<size_t Size, typename Type>
    requires (Size > 0)
    Vec<Size, Type>::Self
    Vec<Size, Type>::operator-(const Type& s) const requires HasSub<Type> {
        Self result;
        for (size_t i = 0; i < Size; ++i) {
            result[i] = data[i] - s;
        }
        return result;
    }

    template<size_t Size, typename Type>
    requires (Size > 0)
    Vec<Size, Type>::Self
    Vec<Size, Type>::operator*(const Type& s) const requires HasMul<Type> {
        Self result;
        for (size_t i = 0; i < Size; ++i) {
            result[i] = data[i] * s;
        }
        return result;
    }

    template<size_t Size, typename Type>
    requires (Size > 0)
    Vec<Size, Type>::Self
    Vec<Size, Type>::operator/(const Type& s) const requires HasDiv<Type> {
        Self result;
        for (size_t i = 0; i < Size; ++i) {
            result[i] = data[i] / s;
        }
        return result;
    }

    template<size_t Size, typename Type>
    requires (Size > 0)
    Vec<Size, Type>::Self
    Vec<Size, Type>::operator<<(const Type& s) const requires HasShl<Type> {
        Self result;
        for (size_t i = 0; i < Size; ++i) {
            result[i] = data[i] << s;
        }
        return result;
    }

    template<size_t Size, typename Type>
    requires (Size > 0)
    Vec<Size, Type>::Self
    Vec<Size, Type>::operator>>(const Type& s) const requires HasShr<Type> {
        Self result;
        for (size_t i = 0; i < Size; ++i) {
            result[i] = data[i] >> s;
        }
        return result;
    }

    template<size_t Size, typename Type>
    requires (Size > 0)
    Vec<Size, Type>::Self
    Vec<Size, Type>::operator&(const Type& s) const requires HasBitAnd<Type> {
        Self result;
        for (size_t i = 0; i < Size; ++i) {
            result[i] = data[i] & s;
        }
        return result;
    }

    template<size_t Size, typename Type>
    requires (Size > 0)
    Vec<Size, Type>::Self
    Vec<Size, Type>::operator|(const Type& s) const requires HasBitOr<Type> {
        Self result;
        for (size_t i = 0; i < Size; ++i) {
            result[i] = data[i] | s;
        }
        return result;
    }

    template<size_t Size, typename Type>
    requires (Size > 0)
    Vec<Size, Type>::Self
    Vec<Size, Type>::operator^(const Type& s) const requires HasBitXor<Type> {
        Self result;
        for (size_t i = 0; i < Size; ++i) {
            result[i] = data[i] ^ s;
        }
        return result;
    }

    template<size_t Size, typename Type>
    requires (Size > 0)
    Vec<Size, Type>::Self
    Vec<Size, Type>::operator&&(const Type& s) const requires HasAnd<Type> {
        Self result;
        for (size_t i = 0; i < Size; ++i) {
            result[i] = data[i] && s;
        }
        return result;
    }

    template<size_t Size, typename Type>
    requires (Size > 0)
    Vec<Size, Type>::Self
    Vec<Size, Type>::operator||(const Type& s) const requires HasOr<Type> {
        Self result;
        for (size_t i = 0; i < Size; ++i) {
            result[i] = data[i] || s;
        }
        return result;
    }

    template<size_t Size, typename Type>
    requires (Size > 0)
    Vec<Size, Type>::Self
    Vec<Size, Type>::operator+(const Vec::Self& other) const requires
    HasAdd<Type> {
        Self result;
        for (size_t i = 0; i < Size; ++i) {
            result[i] = data[i] + other[i];
        }
        return result;
    }

    template<size_t Size, typename Type>
    requires (Size > 0)
    Vec<Size, Type>::Self
    Vec<Size, Type>::operator-(const Vec::Self& other) const requires
    HasSub<Type> {
        Self result;
        for (size_t i = 0; i < Size; ++i) {
            result[i] = data[i] - other[i];
        }
        return result;
    }

    template<size_t Size, typename Type>
    requires (Size > 0)
    Vec<Size, Type>::Self
    Vec<Size, Type>::operator*(const Vec::Self& other) const requires
    HasMul<Type> {
        Self result;
        for (size_t i = 0; i < Size; ++i) {
            result[i] = data[i] * other[i];
        }
        return result;
    }

    template<size_t Size, typename Type>
    requires (Size > 0)
    Vec<Size, Type>::Self
    Vec<Size, Type>::operator/(const Vec::Self& other) const requires
    HasDiv<Type> {
        Self result;
        for (size_t i = 0; i < Size; ++i) {
            result[i] = data[i] / other[i];
        }
        return result;
    }

    template<size_t Size, typename Type>
    requires (Size > 0)
    Vec<Size, Type>::Self
    Vec<Size, Type>::operator<<(const Vec::Self& other) const requires
    HasShl<Type> {
        Self result;
        for (size_t i = 0; i < Size; ++i) {
            result[i] = data[i] << other[i];
        }
        return result;
    }

    template<size_t Size, typename Type>
    requires (Size > 0)
    Vec<Size, Type>::Self
    Vec<Size, Type>::operator>>(const Vec::Self& other) const requires
    HasShr<Type> {
        Self result;
        for (size_t i = 0; i < Size; ++i) {
            result[i] = data[i] >> other[i];
        }
        return result;
    }

    template<size_t Size, typename Type>
    requires (Size > 0)
    Vec<Size, Type>::Self
    Vec<Size, Type>::operator&&(const Vec::Self& other) const requires
    HasAnd<Type> {
        Self result;
        for (size_t i = 0; i < Size; ++i) {
            result[i] = data[i] && other[i];
        }
        return result;
    }

    template<size_t Size, typename Type>
    requires (Size > 0)
    Vec<Size, Type>::Self
    Vec<Size, Type>::operator||(const Vec::Self& other) const requires
    HasOr<Type> {
        Self result;
        for (size_t i = 0; i < Size; ++i) {
            result[i] = data[i] || other[i];
        }
        return result;
    }

    template<size_t Size, typename Type>
    requires (Size > 0)
    Type Vec<Size, Type>::dot(const Vec::Self& other) const {
        return *this % other;
    }

    template<size_t Size, typename Type>
    requires (Size > 0)
    Type
    Vec<Size, Type>::operator%(const Vec::Self& other) const requires (
    HasAdd<Type> && HasMul<Type>) {
        Type v = data[0] * other[0];
        for (size_t i = 1; i < Size; ++i) {
            v += data[i] * other[i];
        }
        return v;
    }

    template<size_t Size, typename Type>
    requires (Size > 0)
    Vec<Size, Type>::Self
    Vec<Size, Type>::cross(const Vec::Self& other) const requires (
    Size == 3 && HasAdd<Type> && HasMul<Type>) {
        return *this ^ other;
    }

    template<size_t Size, typename Type>
    requires (Size > 0)
    Vec<Size, Type>::Self
    Vec<Size, Type>::operator^(const Vec::Self& o) const requires (
    Size == 3 && HasAdd<Type> && HasMul<Type>) {
        return {y() * o.z() - o.y() * z(),
                z() * o.x() - o.z() * x(),
                x() * o.y() - o.x() * y()};
    }

    template<size_t Size, typename Type>
    requires (Size > 0)
    bool Vec<Size, Type>::operator==(const Vec::Self& other) const {
        if (this == &other) return true;
        return std::equal(std::begin(data), std::end(data),
                          std::begin(other.data));
    }

    template<size_t Size, typename Type>
    requires (Size > 0)
    bool Vec<Size, Type>::operator!=(const Vec::Self& other) const {
        if (this == &other) return false;
        return !std::equal(std::begin(data), std::end(data),
                           std::begin(other.data));
    }

    template<size_t Size, typename Type>
    requires (Size > 0)
    Vec<Size, Type>::Storage::iterator Vec<Size, Type>::begin() {
        return data.begin();
    }

    template<size_t Size, typename Type>
    requires (Size > 0)
    Vec<Size, Type>::Storage::iterator Vec<Size, Type>::end() {
        return data.end();
    }

    template<size_t Size, typename Type>
    requires (Size > 0)
    Vec<Size, Type>::Storage::const_iterator Vec<Size, Type>::cbegin() const {
        return data.cbegin();
    }

    template<size_t Size, typename Type>
    requires (Size > 0)
    Vec<Size, Type>::Storage::const_iterator Vec<Size, Type>::cend() const {
        return data.cend();
    }

    template<size_t Size, typename Type>
    requires (Size > 0)
    Vec<Size, Type>::Storage::reverse_iterator Vec<Size, Type>::rbegin() {
        return data.rbegin();
    }

    template<size_t Size, typename Type>
    requires (Size > 0)
    Vec<Size, Type>::Storage::reverse_iterator Vec<Size, Type>::rend() {
        return data.rend();
    }

    template<size_t Size, typename Type>
    requires (Size > 0)
    Vec<Size, Type>::Storage::const_reverse_iterator
    Vec<Size, Type>::crbegin() const {
        return data.crbegin();
    }

    template<size_t Size, typename Type>
    requires (Size > 0)
    Vec<Size, Type>::Storage::const_reverse_iterator
    Vec<Size, Type>::crend() const {
        return data.crend();
    }

}
#endif //RUSH_VEC_IMPL_H
