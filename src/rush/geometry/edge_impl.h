//
// Created by gaelrialcostas on 6/06/24.
//

#ifndef EDGE_IMPL_H
#define EDGE_IMPL_H


#include <rush/geometry/edge_impl.h>

namespace rush {
    template<size_t Dimensions, typename Type>
    template<typename AAllocator, typename BAllocator, typename CAllocator>
    Edge<Dimensions, Type>::Edge(const Vec<Dimensions, Type, AAllocator>& a_,
                                 const Vec<Dimensions, Type, BAllocator>& b_)
        : a(a_), b(b_) {
    }

    template<size_t Dimensions, typename Type>
    template<typename PAllocator>
    Type Edge<Dimensions, Type>::project(
        const Vec<Dimensions, Type, PAllocator>& point) const {
        auto delta = b - a;
        auto lengthSquared = delta.squaredLength();
        return (point - a).dot(delta) / lengthSquared;
    }

    template<size_t Dimensions, typename Type>
    template<typename PAllocator>
    Vec<Dimensions, Type, PAllocator>
    Edge<Dimensions, Type>::pointAt(size_t t) const {
        return a + (b - a) * t;
    }

    template<size_t Dimensions, typename Type>
    template<typename PAllocator>
    Vec<Dimensions, Type, PAllocator>
    Edge<Dimensions, Type>::closestPoint(
        const Vec<Dimensions, Type, PAllocator>& point) const {
        auto projection = project(point);
        if (projection < static_cast<Type>(0)) return a;
        if (projection > static_cast<Type>(1)) return b;
        return pointAt(projection);
    }

    template<size_t Dimensions, typename Type>
    template<typename PAllocator>
    Type Edge<Dimensions, Type>::distanceToPoint(
        const Vec<Dimensions, Type, PAllocator>& point) const {
        auto closest = closestPoint(point);
        return (point - closest).length();
    }

    template<size_t Dimensions, typename Type>
    bool Edge<Dimensions, Type>::operator==(const Edge& other) const {
        return a == other.a && b == other.b;
    }
}

#endif //EDGE_IMPL_H
