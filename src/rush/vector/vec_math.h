//
// Created by gaelr on 12/08/2023.
//

#ifndef RUSH_VEC_MATH_H
#define RUSH_VEC_MATH_H

#include <rush/vector/vec.h>

namespace rush {

    template<size_t Size, typename Type, typename Allocator>
    Vec<Size, Type, Allocator> sqrt(const Vec<Size, Type, Allocator>& v) {
        return {[v](size_t i) { return std::sqrt(v[i]); }};
    }

    template<size_t Size, typename Type, typename Allocator>
    Vec<Size, Type, Allocator> cos(const Vec<Size, Type, Allocator>& v) {
        return {[v](size_t i) { return std::cos(v[i]); }};
    }

    template<size_t Size, typename Type, typename Allocator>
    Vec<Size, Type, Allocator> sin(const Vec<Size, Type, Allocator>& v) {
        return {[v](size_t i) { return std::sin(v[i]); }};
    }

    template<size_t Size, typename Type, typename Allocator>
    Vec<Size, Type, Allocator> tan(const Vec<Size, Type, Allocator>& v) {
        return {[v](size_t i) { return std::tan(v[i]); }};
    }

    template<size_t Size, typename Type, typename Allocator>
    Vec<Size, Type, Allocator> acos(const Vec<Size, Type, Allocator>& v) {
        return {[v](size_t i) { return std::acos(v[i]); }};
    }

    template<size_t Size, typename Type, typename Allocator>
    Vec<Size, Type, Allocator> asin(const Vec<Size, Type, Allocator>& v) {
        return {[v](size_t i) { return std::asin(v[i]); }};
    }

    template<size_t Size, typename Type, typename Allocator>
    Vec<Size, Type, Allocator> atan(const Vec<Size, Type, Allocator>& v) {
        return {[v](size_t i) { return std::atan(v[i]); }};
    }

}

#endif //RUSH_VEC_MATH_H
