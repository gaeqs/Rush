//
// Created by gaelr on 25/01/2024.
//

#ifndef AABB_IMPL_H
#define AABB_IMPL_H

namespace rush {
    template<size_t Dimensions, typename Type, typename VAllocator>
    AABB<Dimensions, Type, VAllocator>::AABB()
        : center(), radius() {
    }

    template<size_t Dimensions, typename Type, typename VAllocator>
    AABB<Dimensions, Type, VAllocator>::AABB(VectorType center_,
                                             VectorType radius_)
        : center(center_), radius(radius_) {
    }


    template<size_t Dimensions, typename Type, typename VAllocator>
    bool AABB<Dimensions, Type, VAllocator>::
    test(const VectorType& vector) const {
        VectorType rad = rush::abs(center - vector);
        for (size_t i = 0; i < Dimensions; i++) {
            if (rad[i] > radius[i]) return false;
        }
        return true;
    }

    template<size_t Dimensions, typename Type, typename VAllocator>
    template<typename OAllocator>
    bool AABB<Dimensions, Type, VAllocator>::
    test(const AABB<Dimensions, Type, OAllocator>& other) const {
        for (size_t i = 0; i < Dimensions; i++) {
            if (std::abs(center[i] - other.center[i]) >
                radius[i] + other.radius[i])
                return false;
        }
        return true;
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
        VectorType radius = center - max;
        return {center, radius};
    }
}
#endif //AABB_IMPL_H
