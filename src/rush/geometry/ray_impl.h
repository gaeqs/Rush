//
// Created by gaelrialcostas on 5/06/24.
//

#ifndef RUSH_RAY_IMPL_H
#define RUSH_RAY_IMPL_H

namespace rush {

    template<size_t Dimensions, typename Type, typename VAllocator>
    Ray<Dimensions, Type, VAllocator>::Ray()
            : origin(), direction() {
    }

    template<size_t Dimensions, typename Type, typename VAllocator>
    Ray<Dimensions, Type, VAllocator>::
    Ray(VectorType origin_, VectorType direction_)
            : origin(origin_), direction(direction_) {

    }

    template<size_t Dimensions, typename Type, typename VAllocator>
    template<typename PAllocator, Algorithm Algorithm>
    Vec<Dimensions, Type, PAllocator>
    Ray<Dimensions, Type, VAllocator>::
    closestPoint(const Vec<Dimensions, Type, PAllocator>& point) const {
        VectorType vec = origin - point;
        auto d = std::max(vec.dot(vec, direction), (Type) 0);
        return origin + d * direction;
    }

    template<size_t Dimensions, typename Type, typename VAllocator>
    bool Ray<Dimensions, Type, VAllocator>::isValid() const {
        return std::abs(direction.squaredLength()) < FLT_EPSILON;
    }

    template<size_t Dimensions, typename Type, typename VAllocator>
    bool Ray<Dimensions, Type, VAllocator>::isNormalized() const {
        return std::abs(direction.squaredLength() - (Type) 1) < FLT_EPSILON;
    }

    template<size_t Dimensions, typename Type, typename VAllocator>
    template<Algorithm Algorithm>
    Ray<Dimensions, Type, VAllocator>
    Ray<Dimensions, Type, VAllocator>::normalized() const {
        return {origin, direction.template normalized<Algorithm>()};
    }

    template<size_t Dimensions, typename Type, typename VAllocator>
    template<typename OAllocator>
    bool Ray<Dimensions, Type, VAllocator>::operator==(
            const Ray<Dimensions, Type, OAllocator>& other) const {
        return origin == other.origin && direction == other.direction;
    }
}

#endif //RUSH_RAY_IMPL_H
