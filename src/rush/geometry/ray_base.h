//
// Created by gaelrialcostas on 5/06/24.
//

#ifndef RUSH_RAY_BASE_H
#define RUSH_RAY_BASE_H

#include <rush/vector/vec.h>


namespace rush {
    /**
     * Represents a ray in a n-dimensional space.
     * @tparam Dimensions the dimensions of the ray.
     * @tparam Type the format of the number used in this ray.
     * @tparam VAllocator the allocator used to store this ray's data.
     */
    template<size_t Dimensions,
        typename Type,
        typename VAllocator = StaticAllocator>
    struct Ray {
        using VectorType = Vec<Dimensions, Type, VAllocator>;

        VectorType origin;
        VectorType direction;

        /**
         * Creates an invalid ray.
         */
        Ray();

        /**
         * Creates a ray using the given origin and direction.
         * @param origin_ the origin.
         * @param direction_ the direction.
         */
        Ray(VectorType origin_, VectorType direction_);

        /**
         * Returns the closest point to the given point that is inside this ray.
         * <p>
         * This method will work even if the vector is not normalized nor valid.
         *
         * @tparam PAllocator the allocator of the vector.
         * @param point the point to test.
         * @return the closest point.
         */
        template<typename PAllocator = StaticAllocator,
            Algorithm Algorithm = Algorithm()>
        Vec<Dimensions, Type, PAllocator>
        closestPoint(const Vec<Dimensions, Type, PAllocator>& point) const;

        /**
         * Returns whether this ray has a valid configuration.
         *
         * A ray has a valid configuration if its direction's module is not 0.
         *
         * @return whether the ray has a valid configuration.
         */
        [[nodiscard]] bool isValid() const;

        /**
        * @return whether this ray's direction is normalized.
        */
        [[nodiscard]] bool isNormalized(
            Type epsilon = static_cast<Type>(0.001)) const;

        /**
         * Returns a normalized version of this ray.
          <p>
         * You can select the precision of the algorithm.
         * A high precision algorithm will provide a high precision
         * result in exchange of more computational cost.
         * A low precision algorithm will be faster, but it may provide
         * poorer results.
         * <p>
         * @tparam Algorithm the algorithm configuration.
         * @return the normalized ray.
         */
        template<Algorithm Algorithm = Algorithm()>
        [[nodiscard]] Ray normalized() const;

        template<typename OAllocator>
        bool operator
        ==(const Ray<Dimensions, Type, OAllocator>& other) const;
    };
}

#include <rush/geometry/ray_impl.h>

#endif //RUSH_RAY_BASE_H
