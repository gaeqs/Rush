//
// Created by gaelr on 27/01/2024.
//

#ifndef COLLIDES_H
#define COLLIDES_H

#include <any>
#include <typeindex>

#include <rush/vector/vec.h>
#include <rush/geometry/aabb_base.h>
#include <rush/geometry/sphere_base.h>
#include <rush/geometry/plane_base.h>
#include <rush/geometry/ray_base.h>
#include <rush/geometry/triangle_base.h>

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

    // Ray - Point
    template<size_t Dimensions,
        typename Type,
        typename AAllocator,
        typename BAllocator>
    [[nodiscard]] bool
    intersects(const Ray<Dimensions, Type, AAllocator>& ray,
               const Vec<Dimensions, Type, BAllocator>& point) {
        return std::abs((ray.closestPoint(point) - point).squaredLength()) <=
               FLT_EPSILON;
    }

    // Ray - Sphere
    template<size_t Dimensions,
        typename Type,
        typename AAllocator,
        typename BAllocator>
    [[nodiscard]] bool
    intersects(const Ray<Dimensions, Type, AAllocator>& ray,
               const Sphere<3, Type, BAllocator>& sphere) {
        constexpr Type ZERO = static_cast<Type>(0);
#ifndef NDEBUG
        if (!ray.isNormalized()) {
            throw std::runtime_error("Ray is not normalized.");
        }
#endif

        Vec<Dimensions, Type> m = ray.origin - sphere.center;
        Type b = m.dot(ray.direction);
        Type c = m.squaredLength() - sphere.radius * sphere.radius;
        if (c > ZERO && b > ZERO) return false;
        return b * b - c > ZERO;
    }

    // Ray - AABB
    template<size_t Dimensions,
        typename Type,
        typename AAllocator,
        typename BAllocator>
    [[nodiscard]] bool
    intersects(const Ray<Dimensions, Type, AAllocator>& ray,
               const AABB<Dimensions, Type, BAllocator>& aabb) {
        constexpr Type ZERO = static_cast<Type>(0);
        constexpr Type ONE = static_cast<Type>(1);

        Type min = std::numeric_limits<Type>::lowest();
        Type max = std::numeric_limits<Type>::max();
        Vec<Dimensions, Type> invD = ONE / ray.direction;
        for (size_t d = 0; d < Dimensions; ++d) {
            Type inv = ONE / ray.direction[d];
            Type t0 = (aabb.center[d] - aabb.radius[d] - ray.origin[d]) * inv;
            Type t1 = (aabb.center[d] + aabb.radius[d] - ray.origin[d]) * inv;
            if (inv < 0.0f) std::swap(t0, t1);
            min = std::max(t0, min);
            max = std::min(t1, max);
            if (max < min || max < ZERO) return false;
        }
        return true;
    }

    // Ray - Plane
    template<typename Type,
        typename AAllocator>
    [[nodiscard]] bool
    intersects(const Ray<3, Type, AAllocator>& ray,
               const Plane<Type>& plane) {
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
        if (nd >= ZERO) return false;
        Type pn = ray.origin.dot(plane.normal);
        return (plane.distance - pn) / nd >= ZERO;
    }

    // Triangle - Point
    template<typename Type,
        typename BAllocator>
    [[nodiscard]] bool
    intersects(const Triangle<Type>& tri,
               const Vec<3, Type, BAllocator>& point) {
        constexpr Type ZERO = static_cast<Type>(0);

        auto a = tri.a - point;
        auto b = tri.b - point;
        auto c = tri.c - point;

        auto u = b.cross(c);
        auto v = c.cross(a);
        auto w = a.cross(b);

        return u.dot(v) > ZERO && u.dot(w) > ZERO;
    }

    // Triangle - AABB
    template<typename Type,
        typename BAllocator>
    [[nodiscard]] bool
    intersects(const Triangle<Type>& tri,
               const AABB<3, Type, BAllocator>& aabb) {
        constexpr Type ZERO = static_cast<Type>(0);
        constexpr Type ONE = static_cast<Type>(1);
        constexpr Type M_ONE = static_cast<Type>(-1);

        static auto projectTri = [](const Triangle<Type>& tri,
                                    Vec<3, Type> axis) {
            Type dotA = axis.dot(tri.a);
            Type dotB = axis.dot(tri.b);
            Type dotC = axis.dot(tri.c);

            Type min = std::min(std::min(dotA, dotB), dotC);
            Type max = std::max(std::max(dotA, dotB), dotC);
            return std::make_pair(min, max);
        };

        static auto projectBox = [](const AABB<3, Type>& box,
                                    Vec<3, Type> axis) {
            Type min = axis.dot(box.center - box.radius);
            Type max = min;

            for (size_t i = 1; i < 8; ++i) {
                Vec<3, Type> mul(
                    (i & 1) == 0 ? M_ONE : ONE,
                    (i & 2) == 0 ? M_ONE : ONE,
                    (i & 4) == 0 ? M_ONE : ONE
                );

                Type dot = axis.dot(box.center + box.radius * mul);
                min = std::min(min, dot);
                max = std::max(max, dot);
            }

            return std::make_pair(min, max);
        };


        // Test box normals

        static Vec<3, Type> NORMALS[3] = {
            Vec<3, Type>(ONE, ZERO, ZERO),
            Vec<3, Type>(ZERO, ONE, ZERO),
            Vec<3, Type>(ZERO, ZERO, ONE)
        };

        for (size_t i = 0; i < 3; ++i) {
            auto [min, max] = projectTri(tri, NORMALS[i]);
            min -= aabb.center[i];
            max -= aabb.center[i];
            if (max < -aabb.radius[i] || min > aabb.radius[i]) {
                return false;
            }
        }


        // Test triangle normal
        {
            Vec<3, Type> normal = tri.normal();
            Type triOffset = normal.dot(tri.a);
            auto [boxMin, boxMax] = projectBox(aabb, normal);
            if (boxMax < triOffset || boxMin > triOffset) return false;
        }

        Vec<3, Type> edges[3] = {
            tri.a - tri.b,
            tri.b - tri.c,
            tri.c - tri.a
        };

        // Test the edges
        for (size_t i = 0; i < 3; ++i) {
            for (size_t j = 0; j < 3; ++j) {
                auto axis = edges[i].cross(NORMALS[j]);
                auto [boxMin, boxMax] = projectBox(aabb, axis);
                auto [triMin, triMax] = projectTri(tri, axis);
                if (boxMax < triMin || boxMin > triMax)return false;
            }
        }

        return true;
    }


    /**
     * This helper function checks if rush::intersects(b, a) exists,
     * swaping both parameters.
     */
    template<typename A, typename B>
    [[nodiscard]] bool intersects(const A& a, const B& b) {
        return rush::intersects(b, a);
    }

    template<typename Other, size_t Dimensions, typename Type>
    [[nodiscard]] bool intersectsAny(const Other& other, const std::any& a) {
        const std::type_index type = a.type();
        if (type == typeid(Ray<Dimensions, Type>)) {
            auto* known = std::any_cast<Ray<Dimensions, Type>>(&a);
            return intersects(*known, other);
        }
        if (type == typeid(AABB<Dimensions, Type>)) {
            auto* known = std::any_cast<AABB<Dimensions, Type>>(&a);
            return intersects(*known, other);
        }
        if (type == typeid(Sphere<Dimensions, Type>)) {
            auto* known = std::any_cast<Sphere<Dimensions, Type>>(&a);
            return intersects(*known, other);
        }
        if (type == typeid(Plane<Type>)) {
            auto* known = std::any_cast<Plane<Type>>(&a);
            return intersects(*known, other);
        }
        if (type == typeid(Vec<Dimensions, Type>)) {
            auto* known = std::any_cast<Vec<Dimensions, Type>>(&a);
            return intersects(*known, other);
        }

        return false;
    }

    template<size_t Dimensions, typename Type>
    [[nodiscard]] bool intersectsAny(const std::any& a, const std::any& b) {
        const std::type_index type = a.type();
        if (type == typeid(Ray<Dimensions, Type>)) {
            auto* known = std::any_cast<Ray<Dimensions, Type>>(&a);
            return intersectsAny<decltype(*known), Dimensions, Type>(*known, b);
        }
        if (type == typeid(AABB<Dimensions, Type>)) {
            auto* known = std::any_cast<AABB<Dimensions, Type>>(&a);
            return intersectsAny<decltype(*known), Dimensions, Type>(*known, b);
        }
        if (type == typeid(Sphere<Dimensions, Type>)) {
            auto* known = std::any_cast<Sphere<Dimensions, Type>>(&a);
            return intersectsAny<decltype(*known), Dimensions, Type>(*known, b);
        }
        if (type == typeid(Plane<Type>)) {
            auto* known = std::any_cast<Plane<Type>>(&a);
            return intersectsAny<decltype(*known), Dimensions, Type>(*known, b);
        }
        if (type == typeid(Vec<Dimensions, Type>)) {
            auto* known = std::any_cast<Vec<Dimensions, Type>>(&a);
            return intersectsAny<decltype(*known), Dimensions, Type>(*known, b);
        }

        return false;
    }
}

#endif //COLLIDES_H
