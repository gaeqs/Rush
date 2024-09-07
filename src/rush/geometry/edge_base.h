//
// Created by gaeqs on 21/07/24.
//

#ifndef EDGE_BASE_H
#define EDGE_BASE_H


#include <rush/vector/vec.h>

namespace rush {
    /**
    * Represents an edge.
    * A edge is represented by two points.
    *
    * @tparam Type the format of the number used in this triangle.
    */
    template<size_t Dimensions, typename Type>
    struct Edge {
        Vec<Dimensions, Type> a;
        Vec<Dimensions, Type> b;

        /**
         * Creates an edge using two points.
         * @tparam AAllocator the allocator of the first point.
         * @tparam BAllocator the allocator of the second point.
         * @param a_ the first point.
         * @param b_ the second point.
         */
        template<typename AAllocator = StaticAllocator,
            typename BAllocator = StaticAllocator,
            typename CAllocator = StaticAllocator>
        Edge(const Vec<Dimensions, Type, AAllocator>& a_,
             const Vec<Dimensions, Type, BAllocator>& b_);

        /**
         * Projects the given point into this edge.
         *
         * @tparam PAllocator the allocator of the vector.
         * @param point the point to project.
         * @return the relative projected value. Use pointAt to get the point.
         */
        template<typename PAllocator = StaticAllocator>
        [[nodiscard]] Type
        project(const Vec<Dimensions, Type, PAllocator>& point) const;

        template<typename PAllocator = StaticAllocator>
        [[nodiscard]] Vec<Dimensions, Type, PAllocator>
        pointAt(size_t t) const;

        /**
         * Returns the closest point to the given point that is inside this edge.
         *
         * @tparam PAllocator the allocator of the vector.
         * @param point the point to test.
         * @return the closest point.
         */
        template<typename PAllocator = StaticAllocator>
        [[nodiscard]] Vec<Dimensions, Type, PAllocator>
        closestPoint(const Vec<Dimensions, Type, PAllocator>& point) const;

        /**
         * Returns the distance between the given point and this edge.
         *
         * @tparam PAllocator the allocator of the vector.
         * @param point the point to test.
         */
        template<typename PAllocator = StaticAllocator>
        [[nodiscard]] Type distanceToPoint(
            const Vec<Dimensions, Type, PAllocator>& point) const;

        bool operator==(const Edge& other) const;
    };
}

#include <rush/geometry/edge_impl.h>

#endif //EDGE_BASE_H
