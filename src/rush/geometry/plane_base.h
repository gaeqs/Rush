//
// Created by gaelr on 27/01/2024.
//

#ifndef PLANE_H
#define PLANE_H

#include <rush/vector/vec.h>

namespace rush {
    template<typename Type>
    struct Plane {
        Vec<3, Type> normal;
        Type distance;

        template<typename NAllocator = rush::StaticAllocator>
        Plane(const Vec<3, Type, NAllocator>& normal_, const Type& distance_);

        template<typename AAllocator = rush::StaticAllocator,
            typename BAllocator = rush::StaticAllocator,
            typename CAllocator = rush::StaticAllocator>
        Plane(const Vec<3, Type, AAllocator>& a,
              const Vec<3, Type, BAllocator>& b,
              const Vec<3, Type, CAllocator>& c);

        template<typename PAllocator = rush::StaticAllocator>
        Vec<3, Type, PAllocator>
        closestPoint(const Vec<3, Type, PAllocator>& point) const;

        template<typename PAllocator = rush::StaticAllocator>
        Type distanceToPoint(const Vec<3, Type, PAllocator>& point) const;

        bool operator==(const Plane& other) const;
    };
}

#include <rush/geometry/plane_impl.h>

#endif //PLANE_H
