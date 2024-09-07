//
// Created by gaelr on 25/01/2024.
//

#ifndef AABB_IMPL_H
#define AABB_IMPL_H
#include <rush/vector/vec_math.h>

namespace rush {
    template<size_t Dimensions, typename Type, typename VAllocator>
    AABB<Dimensions, Type, VAllocator>::AABB()
        : center(), radius() {
    }

    template<size_t Dimensions, typename Type, typename VAllocator>
    AABB<Dimensions, Type, VAllocator>::AABB(VectorType center_,
                                             VectorType radius_)
        : center(center_), radius(abs(radius_)) {
    }

    template<size_t Dimensions, typename Type, typename VAllocator>
    template<typename PAllocator>
    Vec<Dimensions, Type, PAllocator>
    AABB<Dimensions, Type, VAllocator>::closestPoint(
        const Vec<Dimensions, Type, PAllocator>& point) const {
        return rush::clamp(point, center - radius, center + radius);
    }


    template<size_t Dimensions, typename Type, typename VAllocator>
    template<typename OAllocator>
    bool
    AABB<Dimensions, Type, VAllocator>::
    operator==(const AABB<Dimensions, Type, OAllocator>& other) const {
        return center == other.center && radius == other.radius;
    }

    template<size_t Dimensions, typename Type, typename VAllocator>
    AABB<Dimensions, Type, VAllocator>
    AABB<Dimensions, Type, VAllocator>::fromEdges(VectorType a, VectorType b) {
        VectorType min = rush::min(a, b);
        VectorType max = rush::max(a, b);
        VectorType center = (min + max) / Type(2);
        VectorType radius = center - min;
        return {center, radius};
    }
}
#endif //AABB_IMPL_H
