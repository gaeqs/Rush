//
// Created by gaelr on 27/01/2024.
//

#ifndef SPHERE_BASE_H
#define SPHERE_BASE_H

#include <rush/vector/vec.h>

namespace rush {
    template<size_t Dimensions,
        typename Type,
        typename VAllocator = rush::StaticAllocator>
    struct Sphere {
        using VectorType = rush::Vec<Dimensions, Type, VAllocator>;

        VectorType center;
        Type radius;

        Sphere();

        Sphere(VectorType center_, Type radius_);

        template<typename PAllocator = rush::StaticAllocator,
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
