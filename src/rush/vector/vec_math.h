//
// Created by gaelr on 12/08/2023.
//

#ifndef RUSH_VEC_MATH_H
#define RUSH_VEC_MATH_H

#include <numbers>

#include <rush/vector/vec.h>

namespace rush {
    template<size_t Size, typename Type, typename Allocator>
    Vec<Size, Type, Allocator> sqrt(const Vec<Size, Type, Allocator>& v) {
        return Vec<Size, Type, Allocator>{
            [v](size_t i) { return std::sqrt(v[i]); }
        };
    }

    template<size_t Size, typename Type, typename Allocator>
    Vec<Size, Type, Allocator> cos(const Vec<Size, Type, Allocator>& v) {
        return Vec<Size, Type, Allocator>{
            [v](size_t i) { return std::cos(v[i]); }
        };
    }

    template<size_t Size, typename Type, typename Allocator>
    Vec<Size, Type, Allocator> sin(const Vec<Size, Type, Allocator>& v) {
        return Vec<Size, Type, Allocator>{
            [v](size_t i) { return std::sin(v[i]); }
        };
    }

    template<size_t Size, typename Type, typename Allocator>
    Vec<Size, Type, Allocator> tan(const Vec<Size, Type, Allocator>& v) {
        return Vec<Size, Type, Allocator>{
            [v](size_t i) { return std::tan(v[i]); }
        };
    }

    template<size_t Size, typename Type, typename Allocator>
    Vec<Size, Type, Allocator> acos(const Vec<Size, Type, Allocator>& v) {
        return Vec<Size, Type, Allocator>{
            [v](size_t i) { return std::acos(v[i]); }
        };
    }

    template<size_t Size, typename Type, typename Allocator>
    Vec<Size, Type, Allocator> asin(const Vec<Size, Type, Allocator>& v) {
        return Vec<Size, Type, Allocator>{
            [v](size_t i) { return std::asin(v[i]); }
        };
    }

    template<size_t Size, typename Type, typename Allocator>
    Vec<Size, Type, Allocator> atan(const Vec<Size, Type, Allocator>& v) {
        return Vec<Size, Type, Allocator>{
            [v](size_t i) { return std::atan(v[i]); }
        };
    }

    template<size_t Size, typename Type, typename Allocator>
    Vec<Size, Type, Allocator> degrees(const Vec<Size, Type, Allocator>& v) {
        constexpr Type RELATION = Type(180) / std::numbers::pi_v<Type>;
        return Vec<Size, Type, Allocator>{
            [v](size_t i) { return v[i] * RELATION; }
        };
    }

    template<size_t Size, typename Type, typename Allocator>
    Vec<Size, Type, Allocator> radians(const Vec<Size, Type, Allocator>& v) {
        constexpr Type RELATION = std::numbers::pi_v<Type> / Type(180);
        return Vec<Size, Type, Allocator>{
            [v](size_t i) { return v[i] * RELATION; }
        };
    }

    template<size_t Size, typename Type, typename Allocator>
    Vec<Size, Type, Allocator> min(const Vec<Size, Type, Allocator>& v,
                                   Type min) {
        return Vec<Size, Type, Allocator>{
            [v, &min](size_t i) { return std::min(v[i], min); }
        };
    }

    template<size_t Size, typename Type, typename Allocator>
    Vec<Size, Type, Allocator> min(const Vec<Size, Type, Allocator>& v,
                                   const Vec<Size, Type, Allocator>& w) {
        return Vec<Size, Type, Allocator>{
            [v, w](size_t i) { return std::min(v[i], w[i]); }
        };
    }

    template<size_t Size, typename Type, typename Allocator>
    Vec<Size, Type, Allocator> max(const Vec<Size, Type, Allocator>& v,
                                   const Type& max) {
        return Vec<Size, Type, Allocator>{
            [v, max](size_t i) { return std::max(v[i], max); }
        };
    }

    template<size_t Size, typename Type, typename Allocator>
    Vec<Size, Type, Allocator> max(const Vec<Size, Type, Allocator>& v,
                                   const Vec<Size, Type, Allocator>& w) {
        return Vec<Size, Type, Allocator>{
            [v, w](size_t i) { return std::max(v[i], w[i]); }
        };
    }

    template<size_t Size, typename Type, typename Allocator>
    Vec<Size, Type, Allocator> mix(const Vec<Size, Type, Allocator>& a,
                                   const Vec<Size, Type, Allocator>& b,
                                   const Type& x) {
        return b * x + a * (Type(1) - x);
    }

    template<size_t Size, typename Type, typename Allocator>
    Type avg(const Vec<Size, Type, Allocator>& v) {
        Type result = v[0];
        for (size_t i = 1; i < Size; ++i) {
            result += v[i];
        }
        return result / static_cast<Type>(Size);
    }

    template<size_t Size, typename Type, typename Allocator>
    Vec<Size, Type, Allocator>
    pow(const Vec<Size, Type, Allocator>& v, const Type& p) {
        return Vec<Size, Type, Allocator>{
            [v, p](size_t i) { return std::pow(v[i], p); }
        };
    }

    template<size_t Size, typename Type, typename Allocator>
    Vec<Size, Type, Allocator> pow(const Vec<Size, Type, Allocator>& v,
                                   const Vec<Size, Type, Allocator>& p) {
        return Vec<Size, Type, Allocator>{
            [v, p](size_t i) { return std::pow(v[i], p[i]); }
        };
    }
}

#endif //RUSH_VEC_MATH_H
