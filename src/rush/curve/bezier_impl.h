//
// Created by gaelr on 25/09/23.
//

#ifndef NEON_BEZIER_IMPL_H
#define NEON_BEZIER_IMPL_H

template<size_t Size, size_t Dimensions, typename Type, typename Allocator,
        typename PointAllocator>
requires (Size > 0)
template<typename... T>
requires (sizeof...(T) == Size)
rush::BezierSegment<Size, Dimensions, Type, Allocator, PointAllocator>::
BezierSegment(
        T... list) {
    rush::Vec<Dimensions, Type>* ptr = nodes.toPointer();
    ((*ptr++ = list), ...);
}


template<size_t Size, size_t Dimensions, typename Type, typename Allocator,
        typename PointAllocator>
requires (Size > 0)
rush::Vec<Dimensions, Type, PointAllocator>
rush::BezierSegment<Size, Dimensions, Type, Allocator, PointAllocator>::
fetch(Type t) const {
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

template<size_t Size, size_t Dimensions, typename Type, typename Allocator,
        typename PointAllocator>
requires (Size > 0)
template<size_t OSize, typename OAllocator,
        typename OPointAllocator, typename... T>
requires (OSize > 1 && sizeof...(T) == Size - 2)
rush::BezierSegment<Size, Dimensions, Type, Allocator, PointAllocator>
rush::BezierSegment<Size, Dimensions, Type, Allocator, PointAllocator>::
continuousTo(
        rush::BezierSegment<OSize, Dimensions, Type,
                OAllocator, OPointAllocator> o, T... list) {

    rush::BezierSegment<Size, Dimensions, Type, Allocator,
            OPointAllocator> result;

    auto prev = o.nodes[o.nodes.size() - 2];
    auto joint = o.nodes[o.nodes.size() - 1];

    result.nodes[0] = joint;
    result.nodes[1] = joint + (joint - prev);

    rush::Vec<Dimensions, Type>* ptr = result.nodes.toPointer() + 2;
    ((*ptr++ = list), ...);

    return result;
}

template<size_t Segments, size_t Size, size_t Dimensions,
        typename Type, typename Allocator, typename SegmentAllocator,
        typename PointAllocator>
template<typename... T>
requires (sizeof...(T) == Segments)
rush::BezierCurve<Segments, Size, Dimensions, Type,
        Allocator, SegmentAllocator, PointAllocator>::BezierCurve(T... list) {
    Segment* ptr = segments.toPointer();
    ((*ptr++ = list), ...);
}


template<size_t Segments, size_t Size, size_t Dimensions,
        typename Type, typename Allocator, typename SegmentAllocator,
        typename PointAllocator>
rush::Vec<Dimensions, Type>
rush::BezierCurve<Segments, Size, Dimensions, Type,
        Allocator, SegmentAllocator, PointAllocator>::fetch(
        Type t, bool normalized) const {

    if (normalized) {
        // [0, 1] to [0, segments]
        t *= segments.size();
    }

    auto index = std::min(static_cast<size_t>(t), Segments - 1);
    Type offset = t - static_cast<Type>(index);

    return segments[index].fetch(offset);
}


#endif //NEON_BEZIER_IMPL_H
