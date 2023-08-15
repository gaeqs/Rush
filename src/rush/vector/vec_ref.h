//
// Created by gaelr on 31/07/2023.
//

#ifndef RUSH_VEC_REF_H
#define RUSH_VEC_REF_H

#include <iostream>
#include <rush/allocator/allocator.h>

namespace rush {
    template<size_t Size, typename Type, typename Allocator> requires(Size > 0)
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

        template<typename Allocator = StaticAllocator>
        operator Vec<Size, Type, Allocator>() const {
            return toVec<Allocator>();
        }

        template<typename Allocator = StaticAllocator>
        Vec<Size, Type, Allocator> operator*() const {
            return Vec<Size, Type, Allocator>(*this);
        }

        template<typename Allocator = StaticAllocator>
        Vec<Size, Type, Allocator> toVec() const {
            return Vec<Size, Type, Allocator>(*this);
        }

        template<typename Allocator>
        VecRef& operator=(Vec<Size, Type, Allocator> vec) {
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
            Vec<Size, Type, StaticAllocator> vec = ref;
            for (int i = 0; i < Size; ++i) {
                *references[i] = vec[i];
            }
            return *this;
        }

        template<typename AllA, typename AllB = AllA>
        Vec<Size, Type, AllB>
        operator+(const Vec<Size, Type, AllA>& other) {
            return toVec<AllB>() + other;
        }

        template<typename Allocator = StaticAllocator>
        Vec<Size, Type, Allocator> operator+(const VecRef<Size, Type>& other) {
            return **this + *other;
        }

        template<typename AllA, typename AllB = AllA>
        Vec<Size, Type, AllB>
        operator-(const Vec<Size, Type, AllA>& other) {
            return toVec<AllB>() - other;
        }

        template<typename Allocator = StaticAllocator>
        Vec<Size, Type, Allocator> operator-(const VecRef<Size, Type>& other) {
            return **this - *other;
        }

        template<typename AllA, typename AllB = AllA>
        Vec<Size, Type, AllB>
        operator*(const Vec<Size, Type, AllA>& other) {
            return toVec<AllB>() * other;
        }

        template<typename Allocator = StaticAllocator>
        Vec<Size, Type, Allocator> operator*(const VecRef<Size, Type>& other) {
            return **this * *other;
        }

        template<typename AllA, typename AllB = AllA>
        Vec<Size, Type, AllB>
        operator/(const Vec<Size, Type, AllA>& other) {
            return toVec<AllB>() / other;
        }

        template<typename Allocator = StaticAllocator>
        Vec<Size, Type, Allocator> operator/(const VecRef<Size, Type>& other) {
            return **this / *other;
        }

        template<typename AllA, typename AllB = AllA>
        Vec<Size, Type, AllB>
        operator%(const Vec<Size, Type, AllA>& other) {
            return toVec<AllB>() % other;
        }

        template<typename Allocator = StaticAllocator>
        Vec<Size, Type, Allocator> operator%(const VecRef<Size, Type>& other) {
            return **this % *other;
        }

    };

}

#endif