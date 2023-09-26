//
// Created by gaelr on 25/09/23.
//

#ifndef NEON_BEZIER_BASE_H
#define NEON_BEZIER_BASE_H

#include <rush/vector/vec_base.h>

#include <cstddef>

namespace rush {

    template<size_t Size, size_t Dimensions, typename Type,
            typename Allocator = StaticAllocator> requires (Size > 0)
    struct BezierSegment {

        using Storage = Allocator;
        using VectorType = rush::Vec<Dimensions, Type>;

        Allocator::template AllocatedData<Size, VectorType> nodes;

        BezierSegment() = default;

        template<typename... T>
        requires (sizeof...(T) == Size)
        BezierSegment(T... list);

        rush::Vec<Dimensions, Type> fetch(Type t) const;

        // REGION STATIC CONSTRUCTOR

        template<size_t OSize, typename OAllocator, typename... T>
        requires (OSize > 1 && sizeof...(T) == Size - 2)
        static BezierSegment continuousTo(
                BezierSegment<OSize, Dimensions, Type, OAllocator> o,
                T... list);

    };

    template<size_t Segments, size_t Size, size_t Dimensions,
            typename Type,
            typename Allocator = StaticAllocator,
            typename SegmentAllocator = StaticAllocator>
    struct BezierCurve {

        using Storage = Allocator;
        using Segment = BezierSegment<Size, Dimensions, Type, SegmentAllocator>;

        Allocator::template AllocatedData<Segments, Segment> segments;

        template<typename... T>
        requires (sizeof...(T) == Segments)
        BezierCurve(T... list);

        rush::Vec<Dimensions, Type> fetch(Type t, bool normalized) const;

    };
}

#include <rush/curve/bezier_impl.h>

#endif //NEON_BEZIER_BASE_H
