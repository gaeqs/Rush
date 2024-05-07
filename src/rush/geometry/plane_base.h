//
// Created by gaelr on 27/01/2024.
//

#ifndef PLANE_H
#define PLANE_H

#include <rush/vector/vec.h>

namespace rush {
    /**
     * Represents a tridimensional plane.
     * A plane is represented by a normal and a distance to the origin.
     *
     * @tparam Type the format of the number used in this plane.
     */
    template<typename Type>
    struct Plane {
        Vec<3, Type> normal;
        Type distance;

        /**
         * Creates a new plane.
         * @tparam NAllocator the allocator of the normal vector.
         * @param normal_ the normal vector.
         * @param distance_ the distance.
         */
        template<typename NAllocator = StaticAllocator>
        Plane(const Vec<3, Type, NAllocator>& normal_, const Type& distance_);

        /**
         * Creates a plane that contains all three points.
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
        Plane(const Vec<3, Type, AAllocator>& a,
              const Vec<3, Type, BAllocator>& b,
              const Vec<3, Type, CAllocator>& c);

        /**
         * Returns the closest point to the given point that is inside this plane.
         *
         * This method will return an incorrect value if the plane is not normalized.
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
         * Returns the distance between the given point and this plane.
         *
         * This method will return an incorrect value if the plane is not normalized.
         * It will also throw std::runtime_error if the application is in debug mode.
         *
         * @tparam PAllocator the allocator of the vector.
         * @param point the point to test.
         */
        template<typename PAllocator = StaticAllocator>
        [[nodiscard]] Type distanceToPoint(const Vec<3, Type, PAllocator>& point) const;

        /**
         * @return whether this plane's normal is normalized.
         */
        [[nodiscard]] bool isNormalized() const;

        /**
         * Returns a normalzied version of this plane.
          <p>
         * You can select the precision of the algorithm.
         * A high precision algorithm will provide a high precision
         * result in exchange of more computational cost.
         * A low precision algorithm will be faster, but it may provide
         * poorer results.
         * <p>
         * @tparam Algorithm the algorithm configuration.
         * @return the normalized plane.
         */
        template<Algorithm Algorithm = Algorithm()>
        [[nodiscard]] Plane normalized() const;

        bool operator==(const Plane& other) const;
    };
}

#include <rush/geometry/plane_impl.h>

#endif //PLANE_H
