//
// Created by gaelrialcostas on 6/06/24.
//

#ifndef TRIANGLE_BASE_H
#define TRIANGLE_BASE_H

#include <rush/vector/vec.h>

namespace rush {

    /**
    * Represents a triangle.
    * A triangle is represented by a normal and a distance to the origin.
    *
    * @tparam Type the format of the number used in this triangle.
    */
    template<typename Type>
    struct Triangle {
        Vec<3, Type> normal;
        Type distance;

        /**
         * Creates a triangle using three points.
         * @tparam AAllocator the allocator of the first point.
         * @tparam BAllocator the allocator of the second point.
         * @tparam CAllocator the allocator of the third point.
         * @param a the first point.
         * @param b the second point.
         * @param c the third point.
         */
        template<typename AAllocator = StaticAllocator,
            typename BAllocator = StaticAllocator,
            typename CAllocator = StaticAllocator>
        Triangle(const Vec<3, Type, AAllocator>& a,
                 const Vec<3, Type, BAllocator>& b,
                 const Vec<3, Type, CAllocator>& c);

        /**
         * Returns the closest point to the given point that is inside this triangle.
         *
         * This method will return an incorrect value if the triangle is not normalized.
         * It will also throw std::runtime_error if the application is in debug mode.
         *
         * @tparam PAllocator the allocator of the vector.
         * @param point the point to test.
         * @return the closest point.
         */
        template<typename PAllocator = StaticAllocator>
        [[nodiscard]] Vec<3, Type, PAllocator>
        closestPoint(const Vec<3, Type, PAllocator>& point) const;

        /**
         * Returns the distance between the given point and this triangle.
         *
         * This method will return an incorrect value if the triangle is not normalized.
         * It will also throw std::runtime_error if the application is in debug mode.
         *
         * @tparam PAllocator the allocator of the vector.
         * @param point the point to test.
         */
        template<typename PAllocator = StaticAllocator>
        [[nodiscard]] Type distanceToPoint(
            const Vec<3, Type, PAllocator>& point) const;

        bool operator==(const Triangle& other) const;
    };
}

#endif //TRIANGLE_BASE_H
