//
// Created by gaelr on 25/09/23.
//

#ifndef NEON_BEZIER_BASE_H
#define NEON_BEZIER_BASE_H

#include <rush/vector/vec_base.h>

#include <cstddef>

namespace rush {

    /**
     * A segment of a Bézier curve, represented by a list
     * of points.
     * <p>
     * The Bézier curve only guarantees that the first and last
     * is gong to be traveled.
     * <p>
     * The recommended size of a Bézier segment is of four
     * points.
     * You can concatenate several segments using a BezierCurve
     * structure.
     *
     * @tparam Size the amount of points of the segment.
     * @tparam Dimensions the dimension of the points.
     * @tparam Type the data type inside the points.
     * @tparam Allocator the allocator used in the list.
     * @tparam PointAllocator the allocator used in the vectors.
     */
    template<size_t Size, size_t Dimensions, typename Type,
            typename Allocator = StaticAllocator,
            typename PointAllocator = StaticAllocator> requires (Size > 0)
    struct BezierSegment {

        using Storage = Allocator;
        using VectorType = rush::Vec<Dimensions, Type, PointAllocator>;

        Allocator::template AllocatedData<Size, VectorType> nodes;

        BezierSegment() = default;

        template<typename... T>
        requires (sizeof...(T) == Size)
        BezierSegment(T... list);

        /**
         * Fetches a point inside the curve at the given
         * timestamp.
         * <p>
         * The timestamp should be inside the range [0, 1].
         * The timestamp 0 gives
         * you the start of the curve, while timestamp 1 gives
         * you the end.
         * Using a timestamp outside this range gives a undefined
         * behaviour.
         *
         * @param t the timestamp.
         * @return the point inside the curve.
         */
        rush::Vec<Dimensions, Type, PointAllocator> fetch(Type t) const;

        // REGION STATIC CONSTRUCTOR

        /**
         * Creates a Bézier segment continuous to the given
         * Bézier segment and the following given points.
         * <p>
         * The first two points of the new segment is generated
         * using the information of the given segment, making
         * the new segment continuous to the given one.
         * <p>
         * The next points are used to generate the rest of the
         * Bézier curve.
         * @tparam OSize the size of the new segment.
         * @tparam OAllocator the allocator of the new segment.
         * @tparam OPointAllocator the point allocator of the new segment.
         * @tparam T the type of the point.
         * @param o the previous segment.
         * @param list the list of points.
         * @return the new segment.
         */
        template<size_t OSize, typename OAllocator,
                typename OPointAllocator, typename... T>
        requires (OSize > 1 && sizeof...(T) == Size - 2)
        static BezierSegment continuousTo(
                BezierSegment<OSize, Dimensions, Type, OAllocator,
                        OPointAllocator> o,
                T... list);

    };

    /**
     * A Bézier curve, represented by a list of segments.
     * The segments are concatenated, and they're not
     * guaranteed to be continuous.
     *
     * @tparam Segments the amount of segments of the curve.
     * @tparam Size the amount of points in each segment.
     * @tparam Dimensions the amount of dimensions of each point.
     * @tparam Type the data type of the points.
     * @tparam Allocator the allocator for the curve.
     * @tparam SegmentAllocator the allocator for each segment.
     * @tparam PointAllocator the allocator for each point.
     */
    template<size_t Segments, size_t Size, size_t Dimensions,
            typename Type,
            typename Allocator = StaticAllocator,
            typename SegmentAllocator = StaticAllocator,
            typename PointAllocator = StaticAllocator>
    struct BezierCurve {

        using Storage = Allocator;
        using Segment = BezierSegment<Size, Dimensions, Type,
                SegmentAllocator, PointAllocator>;

        Allocator::template AllocatedData<Segments, Segment> segments;

        template<typename... T>
        requires (sizeof...(T) == Segments)
        BezierCurve(T... list);

        /**
         * Fetches a point inside the curve at the given
         * timestamp.
         * <p>
         * If normalized, the timestamp should be inside the range [0, 1].
         * The timestamp 0 gives
         * you the start of the curve, while timestamp 1 gives
         * you the end.
         * Using a timestamp outside this range gives a undefined
         * behaviour.
         * <p>
         * The non-normalized range is [0, n], being <i>n</i> the
         * amount of segments of this curve.
         *
         * @param t the timestamp.
         * @return the point inside the curve.
         */
        rush::Vec<Dimensions, Type> fetch(Type t, bool normalized) const;

    };
}

#include <rush/curve/bezier_impl.h>

#endif //NEON_BEZIER_BASE_H