//
// Created by gaelr on 25/09/23.
//

#ifndef NEON_BEZIER_IMPL_H
#define NEON_BEZIER_IMPL_H

template<size_t Size, size_t Dimensions, typename Type, typename Allocator>
requires (Size > 0)
template<typename... T>
requires std::is_convertible_v<std::common_type_t<T...>,
        rush::Vec<Dimensions, Type>> && (sizeof...(T) <= Size)
rush::BezierSegment<Size, Dimensions, Type, Allocator>::BezierSegment(
        T... list) {
    rush::Vec<Dimensions, Type>* ptr = nodes.toPointer();
    ((*ptr++ = list), ...);
}


template<size_t Size, size_t Dimensions, typename Type, typename Allocator>
requires (Size > 0)
rush::Vec<Dimensions, Type>
rush::BezierSegment<Size, Dimensions, Type, Allocator>::fetch(Type t) const {
    if constexpr (Size == 1) {
        return nodes[0];
    }
    if constexpr (Size == 2) {
        return nodes[0] + (nodes[1] - nodes[0]) * t;
    }
    if constexpr (Size == 3) {
        Type m = Type(1) - t;
        return m * m * nodes[0]
               + Type(2) * t * m * nodes[1]
               + t * t * nodes[2];
    }
    if constexpr (Size == 4) {
        Type m = Type(1) - t;
        return m * m * m * nodes[0]
               + Type(3) * t * m * m * nodes[1]
               + Type(3) * t * t * m * nodes[2]
               + t * t * t * nodes[3];
    }

    auto bin = [](size_t i, size_t n, Type t) {
        Type b = rush::binomial(n, i);
        for (size_t p = 0; p < i; ++p) {
            b *= t;
        }
        for (size_t p = 0; p < n - i; ++p) {
            b *= (Type(1) - t);
        }
        return b;
    };

    Type m = Type(1) - t;

    rush::Vec<Dimensions, Type> result;
    for (size_t i = 0; i < Size; ++i) {
        result += nodes[i] * bin(i, Size - 1, t);
    }

    return result;
}

#endif //NEON_BEZIER_IMPL_H
