//
// Created by gaelr on 27/01/2024.
//

#ifndef SPHERE_BASE_H
#define SPHERE_BASE_H

#include <rush/vector/vec.h>

namespace rush {
    /**
     * Represents a N-dimensional sphere.
     * @tparam Dimensions the dimensions of the sphere.
     * @tparam Type the format of the number used in this sphere.
     * @tparam VAllocator the allocator used to storage the data.
     */
    template<size_t Dimensions,
        typename Type,
        typename VAllocator = StaticAllocator>
    struct Sphere {
        using VectorType = Vec<Dimensions, Type, VAllocator>;

        VectorType center;
        Type radius;

        /**
         * Creates a new sphere at the origin with radius 0.
         */
        Sphere();

        /**
         * Creates a new sphere.
         * @param center_ the center of the sphere.
         * @param radius_ the radius.
         */
        Sphere(VectorType center_, Type radius_);

        /**
         * Returns the closest point to the given point that is inside this sphere.
         * @tparam PAllocator the allocator of the given point.
         * @param point the point to test.
         * @return the closest point.
         */
        template<typename PAllocator = StaticAllocator,
            Algorithm Algorithm = Algorithm()>
        Vec<Dimensions, Type, PAllocator>
        closestPoint(const Vec<Dimensions, Type, PAllocator>& point) const;

        template<typename OAllocator>
        bool operator
        ==(const Sphere<Dimensions, Type, OAllocator>& other) const;
    };
}

#include <rush/geometry/sphere_impl.h>

#endif //SPHERE_BASE_H
