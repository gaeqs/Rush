//
// Created by gaelr on 25/01/2024.
//

#ifndef AABB_H
#define AABB_H

#include <rush/vector/vec.h>

namespace rush {
    template<size_t Dimensions,
        typename Type,
        typename VAllocator = rush::StaticAllocator>
    struct AABB {
        using VectorType = rush::Vec<Dimensions, Type, VAllocator>;

        VectorType center;
        VectorType radius;

        AABB();

        AABB(VectorType center_, VectorType radius_);

        [[nodiscard]] bool test(const VectorType& vector) const;

        template<typename OAllocator>
        [[nodiscard]] bool
        test(const AABB<Dimensions, Type, OAllocator>& other) const;

        template<typename OAllocator>
        bool operator==(const AABB<Dimensions, Type, OAllocator>& other) const;

        [[nodiscard]] static AABB fromEdges(VectorType a, VectorType b);
    };
}

#include <rush/bound/aabb_impl.h>

#endif //AABB_H
