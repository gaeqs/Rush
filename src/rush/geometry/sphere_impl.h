//
// Created by gaelr on 27/01/2024.
//

#ifndef SPHERE_IMPL_H
#define SPHERE_IMPL_H

namespace rush {
    template<size_t Dimensions, typename Type, typename VAllocator>
    Sphere<Dimensions, Type, VAllocator>::Sphere()
        : center(), radius() {
    }

    template<size_t Dimensions, typename Type, typename VAllocator>
    Sphere<Dimensions, Type, VAllocator>::Sphere(VectorType center_,
                                                 Type radius_)
        : center(center_), radius(radius_) {
    }

    template<size_t Dimensions, typename Type, typename VAllocator>
    template<typename PAllocator, Algorithm Algorithm>
    Vec<Dimensions, Type, PAllocator>
    Sphere<Dimensions, Type, VAllocator>::closestPoint(
        const Vec<Dimensions, Type, PAllocator>& point) const {
        auto local = center - point;
        auto localEdge = local.template normalized<Algorithm>() * radius;
        return localEdge + center;
    }


    template<size_t Dimensions, typename Type, typename VAllocator>
    template<typename OAllocator>
    bool
    Sphere<Dimensions, Type, VAllocator>::
    operator==(const Sphere<Dimensions, Type, OAllocator>& other) const {
        return center == other.center && radius == other.radius;
    }
}

#endif //SPHERE_IMPL_H
