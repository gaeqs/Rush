//
// Created by gaelr on 27/01/2024.
//

#ifndef RAYCAST_H
#define RAYCAST_H

#include <any>
#include <limits>
#include <string>

#include <rush/vector/vec.h>
#include <rush/geometry/aabb_base.h>
#include <rush/geometry/sphere_base.h>
#include <rush/geometry/plane_base.h>

namespace rush {
    template<size_t Dimensions, typename Type>
    struct RayCastResult {
        Vec<Dimensions, Type> point;
        Vec<Dimensions, Type> normal;
        Type distance = std::numeric_limits<Type>::max();
        bool hit = false;
    };


    // Ray - Sphere
    template<size_t Dimensions,
        typename Type,
        typename AAllocator,
        typename BAllocator>
    void raycast(const Ray<Dimensions, Type, AAllocator>& ray,
                 const Sphere<3, Type, BAllocator>& sphere,
                 RayCastResult<Dimensions, Type>& result) {
        constexpr Type ZERO = static_cast<Type>(0);
#ifndef NDEBUG
        if (!ray.isNormalized()) {
            throw std::runtime_error("Ray is not normalized.");
        }
#endif

        Vec<Dimensions, Type> direction = ray.origin - sphere.center;
        Type b = direction.dot(ray.direction);
        Type offset = direction.squaredLength() - sphere.radius * sphere.radius;
        if (offset > ZERO && b > ZERO) {
            result.hit = false;
            return;
        }

        Type discriminant = b * b - offset;
        if (discriminant < 0.0f) {
            result.hit = false;
            return;
        }

        result.distance = std::max(-b - std::sqrt(discriminant), ZERO);
        result.point = ray.origin + result.distance * ray.direction;
        result.normal = (result.point - sphere.center) / sphere.radius;
        result.hit = true;
    }

    // Ray - Sphere
    template<size_t Dimensions,
        typename Type,
        typename AAllocator,
        typename BAllocator>
    void raycast(const Ray<Dimensions, Type, AAllocator>& ray,
                 const AABB<3, Type, BAllocator>& aabb,
                 RayCastResult<Dimensions, Type>& result) {
        constexpr Type ZERO = static_cast<Type>(0);
        constexpr Type ONE = static_cast<Type>(1);

#ifndef NDEBUG
        if (!ray.isNormalized()) {
            throw std::runtime_error(
                "Ray is not normalized. Ray direction length: " +
                std::to_string(ray.direction.length()));
        }
#endif

        size_t minIndex = 0;
        size_t maxIndex = 0;
        Type min = std::numeric_limits<Type>::lowest();
        Type max = std::numeric_limits<Type>::max();
        Vec<Dimensions, Type> invD = ONE / ray.direction;
        for (size_t d = 0; d < Dimensions; ++d) {
            Type inv = ONE / ray.direction[d];
            Type t0 = (aabb.center[d] - aabb.radius[d] - ray.origin[d]) * inv;
            Type t1 = (aabb.center[d] + aabb.radius[d] - ray.origin[d]) * inv;
            if (inv < 0.0f) std::swap(t0, t1);
            if (min < t0) {
                minIndex = d;
                min = t0;
            }
            if (max > t1) {
                maxIndex = d;
                max = t1;
            }
            if (max < min || max < ZERO) {
                result.hit = false;
                return;
            }
        }

        size_t index = min > ZERO ? minIndex : maxIndex;
        Vec<Dimensions, Type> normal;
        normal[index] = min > ZERO ? -ONE : ONE;

        result.distance = min > ZERO ? min : max;
        result.point = ray.origin + result.distance * ray.direction;
        result.normal = normal;
        result.hit = true;
    }

    // Ray - Plane
    template<size_t Dimensions,
        typename Type,
        typename AAllocator>
    void raycast(const Ray<Dimensions, Type, AAllocator>& ray,
                 const Plane<Type>& plane,
                 RayCastResult<Dimensions, Type>& result) {
        constexpr Type ZERO = static_cast<Type>(0);
        constexpr Type ONE = static_cast<Type>(1);

#ifndef NDEBUG
        if (!ray.isNormalized()) {
            throw std::runtime_error("Ray is not normalized.");
        }
        if (!plane.isNormalized()) {
            throw std::runtime_error("Ray is not normalized.");
        }
#endif
        Type nd = ray.direction.dot(plane.normal);
        if (nd >= ZERO) {
            result.hit = false;
            return;
        }
        Type pn = ray.origin.dot(plane.normal);
        Type t = (plane.distance - pn) / nd;

        result.hit = t >= ZERO;
        result.distance = t;
        result.normal = plane.normal;
        result.point = ray.origin + t * ray.direction;
    }

    // Ray - Triangle
    template<size_t Dimensions,
        typename Type,
        typename AAllocator>
    void raycast(const Ray<Dimensions, Type, AAllocator>& ray,
                 const Triangle<Type>& tri,
                 RayCastResult<Dimensions, Type>& result) {
        constexpr Type ZERO = static_cast<Type>(0);
        constexpr Type ONE = static_cast<Type>(1);
        constexpr Type EPS = static_cast<Type>(EPSILON);

#ifndef NDEBUG
        if (!ray.isNormalized()) {
            throw std::runtime_error("Ray is not normalized.");
        }
#endif

        Vec<3, Type> ab = tri.b - tri.a;
        Vec<3, Type> ac = tri.c - tri.a;
        Vec<3, Type> n = ab.cross(ac);

        Type d = -ray.direction.dot(n);
        if (d <= ZERO) {
            result.hit = false;
            return;
        }

        Vec<3, Type> ap = ray.origin - tri.a;
        Type t = ap.dot(n);
        if (t < ZERO) {
            result.hit = false;
            return;
        }

        Vec<3, Type> e = (-ray.direction).cross(ap);
        Type v = ac.dot(e);
        if (v < ZERO || v > d) {
            result.hit = false;
            return;
        }

        Type w = -ab.dot(e);
        if (w < ZERO || v + w > d) {
            result.hit = false;
            return;
        }

        Type invD = ONE / d;
        t *= invD;

        // Barycentric coordinates.
        //v *= invD;
        //w *= invD;
        //Type u = ONE - v - w;

        result.hit = true;
        result.point = ray.origin + t * ray.direction;
        result.distance = t;
        result.normal = n.normalized();
    }
}

#endif //RAYCAST_H
