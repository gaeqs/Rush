//
// Created by gaelr on 25/01/2024.
//

#ifndef AABB_H
#define AABB_H

#include <rush/vector/vec.h>

namespace rush {
    /**
     * Represents an axis-aligned bounding box.
     * @tparam Dimensions the dimensions of the box.
     * @tparam Type the format of the number used in this box.
     * @tparam VAllocator the allocator used to store this box's data.
     */
    template<size_t Dimensions,
        typename Type,
        typename VAllocator = StaticAllocator>
    struct AABB {
        using VectorType = Vec<Dimensions, Type, VAllocator>;

        VectorType center;
        VectorType radius;

        /**
         * Creates a new AABB with radius 0 and the center in the origin.
         */
        AABB();

        /**
         * Creates a new AABB using the given center and radius.
         * @param center_ the center.
         * @param radius_ the radius.
         */
        AABB(VectorType center_, VectorType radius_);

        /**
         * Returns the closest point to the given point that is inside this box.
         * @tparam PAllocator the allocator of the given point.
         * @param point the point to test.
         * @return the closest point.
         */
        template<typename PAllocator = StaticAllocator>
        Vec<Dimensions, Type, PAllocator>
        closestPoint(const Vec<Dimensions, Type, PAllocator>& point) const;


        template<typename OAllocator>
        bool operator==(const AABB<Dimensions, Type, OAllocator>& other) const;

        /**
         * Creates a new AABB with the given edges.
         * @param a the first edge.
         * @param b the second edge.
         * @return the new AABB.
         */
        [[nodiscard]] static AABB fromEdges(VectorType a, VectorType b);
    };
}

#include <rush/geometry/aabb_impl.h>

#endif //AABB_H
