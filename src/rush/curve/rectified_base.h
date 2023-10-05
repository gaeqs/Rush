//
// Created by gaelr on 5/10/23.
//

#ifndef NEON_RECTIFIED_BASE_H
#define NEON_RECTIFIED_BASE_H

#include <rush/allocator/stack_allocator.h>

#include <cstddef>

namespace rush {

    /**
     * A curve, rectified to have a constant velocity
     * throughout all its valid range.
     *
     * @tparam Segments the amount of segments of the curve.
     * @tparam Size the amount of points in each segment.
     * @tparam Dimensions the amount of dimensions of each point.
     * @tparam Samples the amount of rectification samples.
     * @tparam Type the data type of the points.
     * @tparam Allocator the allocator for the rectification information.
     * @tparam CurveAllocator the allocator for the curve.
     * @tparam SegmentAllocator the allocator for each segment.
     * @tparam PointAllocator the allocator for each point.
     */
    template<size_t Samples, typename Type, typename Curve,
            typename Allocator = StaticAllocator>
    struct RectifiedCurve {

        Curve curve;
        Allocator::template AllocatedData<Samples, Type> samples;

        explicit RectifiedCurve(const Curve& c);

        explicit RectifiedCurve(Curve&& c);

        auto fetch(Type t) const;

        auto fetchDerivative(Type t) const;

    private:

        void populate();

    };
}

#include <rush/curve/rectified_impl.h>

#endif //NEON_RECTIFIED_BASE_H
