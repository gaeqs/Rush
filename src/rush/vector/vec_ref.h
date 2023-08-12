//
// Created by gaelr on 31/07/2023.
//

#ifndef RUSH_VEC_REF_H
#define RUSH_VEC_REF_H

namespace rush {
    template<size_t Size, typename Type> requires(Size > 0)
    struct Vec;

    template<size_t Size, typename Type> requires (Size > 0)
    struct VecRef {

        Type* references[Size];

        VecRef() {
        }

        template<typename... T>
        requires(sizeof...(T) > 0)
        explicit VecRef(T... list) : references{list...} {
        }

        operator Vec<Size, Type>() const {
            return **this;
        }

        Vec<Size, Type> operator*() const {
            return Vec<Size, Type>(*this);
        }

        VecRef& operator=(Vec<Size, Type> vec) {
            // Ask for a value, not a reference!
            // This avoids aliasing.

            for (int i = 0; i < Size; ++i) {
                *references[i] = vec[i];
            }
            return *this;
        }

        VecRef& operator=(const VecRef<Size, Type>& ref) {
            if (&ref == this) return *this;

            // We must transform the reference to avoid aliasing.
            // o(0, 1) = o(1, 0) will fail if this is not done.
            Vec<Size, Type> vec = ref;

            for (int i = 0; i < Size; ++i) {
                *references[i] = vec[i];
            }
            return *this;
        }

        Vec<Size, Type> operator+(const Vec<Size, Type>& other) {
            return **this + other;
        }

        Vec<Size, Type> operator+(const VecRef<Size, Type>& other) {
            return **this + *other;
        }

        Vec<Size, Type> operator-(const Vec<Size, Type>& other) {
            return **this - other;
        }

        Vec<Size, Type> operator-(const VecRef<Size, Type>& other) {
            return **this - *other;
        }

        Vec<Size, Type> operator*(const Vec<Size, Type>& other) {
            return **this * other;
        }

        Vec<Size, Type> operator*(const VecRef<Size, Type>& other) {
            return **this * *other;
        }

        Vec<Size, Type> operator/(const Vec<Size, Type>& other) {
            return **this / other;
        }

        Vec<Size, Type> operator/(const VecRef<Size, Type>& other) {
            return **this / *other;
        }

        Vec<Size, Type> operator%(const Vec<Size, Type>& other) {
            return **this % other;
        }

        Vec<Size, Type> operator%(const VecRef<Size, Type>& other) {
            return **this % *other;
        }

    };

}

#endif