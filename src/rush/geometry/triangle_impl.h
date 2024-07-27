//
// Created by gaelrialcostas on 6/06/24.
//

#ifndef TRIANGLE_IMPL_H
#define TRIANGLE_IMPL_H

#include <rush/geometry/edge_base.h>
#include <rush/geometry/intersections.h>

namespace rush {
    template<typename Type>
    template<Algorithm Algorithm>
    Vec<3, Type> Triangle<Type>::normal() const {
        auto ab = b - a;
        auto ac = c - a;
        return ab.cross(ac).template normalized<Algorithm>();
    }

    template<typename Type>
    template<typename AAllocator, typename BAllocator, typename CAllocator>
    Triangle<Type>::Triangle(const Vec<3, Type, AAllocator>& a_,
                             const Vec<3, Type, BAllocator>& b_,
                             const Vec<3, Type, CAllocator>& c_)
        : a(a_), b(b_), c(c_) {
    }

    template<typename Type>
    template<typename PAllocator>
    Vec<3, Type, PAllocator>
    Triangle<Type>::closestPoint(const Vec<3, Type, PAllocator>& point) const {
        constexpr Type ZERO = static_cast<Type>(0);
        constexpr Type ONE = static_cast<Type>(1);

        auto plane = rush::Plane<Type>(a, b, c);
        auto projected = plane.closestPoint(point);

        if (intersects(*this, point)) return point;

        auto ab = rush::Edge<3, Type>(a, b);
        auto bc = rush::Edge<3, Type>(b, c);
        auto ca = rush::Edge<3, Type>(c, a);

        auto c1 = ab.closestPoint(point);
        auto c2 = bc.closestPoint(point);
        auto c3 = ca.closestPoint(point);

        Type l1 = (point - c1).squaredLength();
        Type l2 = (point - c2).squaredLength();
        Type l3 = (point - c3).squaredLength();

        Type min = std::min(l1, std::min(l2, l3));
        if (min == l1) return c1;
        if (min == l2) return c2;
        return c3;
    }

    template<typename Type>
    template<typename PAllocator>
    Type Triangle<Type>::distanceToPoint(
        const Vec<3, Type, PAllocator>& point) const {
        auto closest = closestPoint(point);
        return (point - closest).length();
    }

    template<typename Type>
    bool Triangle<Type>::operator==(const Triangle& other) const {
        return a == other.a && b == other.b && c == other.c;
    }
}

#endif //TRIANGLE_IMPL_H
