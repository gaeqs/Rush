//
// Created by gaelr on 27/01/2024.
//

#ifndef PLANE_IMPL_H
#define PLANE_IMPL_H
#include <float.h>

namespace rush {
    template<typename Type>
    template<typename NAllocator>
    Plane<Type>::Plane(const Vec<3, Type, NAllocator>& normal_,
                       const Type& distance_)
        : normal(normal_),
          distance(distance_) {
    }

    template<typename Type>
    template<typename AAllocator,
        typename BAllocator,
        typename CAllocator>
    Plane<Type>::Plane(const Vec<3, Type, AAllocator>& a,
                       const Vec<3, Type, BAllocator>& b,
                       const Vec<3, Type, CAllocator>& c)
        : normal((b - a).cross(c - a).normalized()),
          distance(normal.dot(a)) {
    }

    template<typename Type>
    template<typename PAllocator>
    Vec<3, Type, PAllocator>
    Plane<Type>::closestPoint(const Vec<3, Type, PAllocator>& point) const {
#ifndef NDEBUG
        if (!isNormalized()) {
            throw std::runtime_error("Plane is not normalized.");
        }
#endif
        return point - distanceToPoint(point) * normal;
    }

    template<typename Type>
    template<typename PAllocator>
    Type
    Plane<Type>::distanceToPoint(const Vec<3, Type, PAllocator>& point) const {
#ifndef NDEBUG
        if (!isNormalized()) {
            throw std::runtime_error("Plane is not normalized.");
        }
#endif
        return normal.dot(point) - distance;
    }

    template<typename Type>
    bool Plane<Type>::isNormalized() const {
        return std::abs(normal.squaredLength() - 1.0f) < FLT_EPSILON;
    }


    template<typename Type>
    template<Algorithm Algorithm>
    Plane<Type> Plane<Type>::normalized() const {
        Type length = normal.template inverseLength<Type, Algorithm>();
        return Plane(normal * length, distance * length);
    }

    template<typename Type>
    bool Plane<Type>::operator==(const Plane& other) const {
        return normal == other.normal && distance == other.distance;
    }
}

#endif //PLANE_IMPL_H
