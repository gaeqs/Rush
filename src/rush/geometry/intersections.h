//
// Created by gaelr on 27/01/2024.
//

#ifndef COLLIDES_H
#define COLLIDES_H

#include <rush/vector/vec.h>
#include <rush/geometry/aabb_base.h>
#include <rush/geometry/sphere_base.h>

namespace rush {
    // AABB - Point
    template<size_t Dimensions,
        typename Type,
        typename AAllocator,
        typename BAllocator>
    [[nodiscard]] bool
    intersects(const AABB<Dimensions, Type, AAllocator>& aabb,
               const Vec<Dimensions, Type, BAllocator>& point) {
        Vec rad = rush::abs(aabb.center - point);
        for (size_t i = 0; i < Dimensions; i++) {
            if (rad[i] > aabb.radius[i]) return false;
        }
        return true;
    }

    // ABBB - AABB
    template<size_t Dimensions,
        typename Type,
        typename AAllocator,
        typename BAllocator>
    [[nodiscard]] bool
    intersects(const AABB<Dimensions, Type, AAllocator>& a,
               const AABB<Dimensions, Type, BAllocator>& b) {
        for (size_t i = 0; i < Dimensions; i++) {
            if (std::abs(a.center[i] - b.center[i]) >
                a.radius[i] + b.radius[i])
                return false;
        }
        return true;
    }

    // Sphere - Point
    template<size_t Dimensions,
        typename Type,
        typename AAllocator,
        typename BAllocator>
    [[nodiscard]] bool
    intersects(const Sphere<Dimensions, Type, AAllocator>& sphere,
               const Vec<Dimensions, Type, BAllocator>& point) {
        return sphere.radius * sphere.radius >=
               (point - sphere.center).squaredLength();
    }

    // Sphere - Sphere
    template<size_t Dimensions,
        typename Type,
        typename AAllocator,
        typename BAllocator>
    [[nodiscard]] bool
    intersects(const Sphere<Dimensions, Type, AAllocator>& a,
               const Sphere<Dimensions, Type, BAllocator>& b) {
        float distanceSq = a.center.dot(b.center);
        float radiusSum = a.radius + b.radius;
        return distanceSq <= radiusSum * radiusSum;
    }

    // AABB - Sphere
    template<size_t Dimensions,
        typename Type,
        typename AAllocator,
        typename BAllocator>
    [[nodiscard]] bool
    intersects(const AABB<Dimensions, Type, AAllocator>& a,
               const Sphere<Dimensions, Type, BAllocator>& b) {
        return rush::intersects(b, a.closestPoint(b.center));
    }

    // Plane - Point
    template<typename Type, typename BAllocator>
    [[nodiscard]] bool
    intersects(const Plane<Type>& plane,
               const Vec<3, Type, BAllocator>& point,
               Type epsilon = Type(0.0001)) {
        return std::abs(plane.normal.dot(point) - plane.distance) <= epsilon;
    }

    // Plane - Sphere
    template<typename Type, typename BAllocator>
    [[nodiscard]] bool
    intersects(const Plane<Type>& plane,
               const Sphere<3, Type, BAllocator>& sphere) {
        auto point = plane.closestPoint(sphere.center);
        auto distanceSq = (point - sphere.center).squaredLength();
        return distanceSq <= sphere.radius * sphere.radius;
    }

    // Plane - AABB
    template<typename Type, typename BAllocator>
    [[nodiscard]] bool
    intersects(const Plane<Type>& plane,
               const AABB<3, Type, BAllocator>& aabb) {
        auto n = rush::abs(plane.normal);
        auto r = aabb.radius.dot(n);
        auto s = plane.normal.dot(aabb.center) - plane.distance;
        return s <= r && s >= -r;
    }

    // Plane - Plane
    template<typename Type>
    [[nodiscard]] bool intersects(const Plane<Type>& a,
                                  const Plane<Type>& b,
                                  Type epsilon = Type(0.0001)) {
        return a.normal.cross(b.normal).squaredLength() >= epsilon;
    }

    /**
     * This helper function checks if rush::intersects(b, a) exists,
     * swaping both parameters.
     */
    template<typename A, typename B>
    [[nodiscard]] bool intersects(const A& a, const B& b) {
        return rush::intersects(b, a);
    }
}

#endif //COLLIDES_H
