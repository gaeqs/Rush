//
// Created by gaelr on 25/09/23.
//

#ifndef NEON_BEZIER_BASE_H
#define NEON_BEZIER_BASE_H

#include <rush/vector/vec_base.h>

#include <cstddef>

namespace rush {

    template<size_t Size, size_t Dimensions, typename Type,
            typename Allocator = StaticAllocator> requires (Size > 0)
    struct BezierSegment {

        using Storage = Allocator;
        using VectorType =  rush::Vec<Dimensions, Type>;

        Allocator::template AllocatedData<Size, VectorType> nodes;

        template<typename... T>
        requires std::is_convertible_v<std::common_type_t<T...>,
                rush::Vec<Dimensions, Type>> &&
                 (sizeof...(T) <= Size)
        BezierSegment(T... list);


        rush::Vec<Dimensions, Type> fetch(Type t) const;

    };

    struct BezierCurve {

    };

}

#include <rush/curve/bezier_impl.h>

#endif //NEON_BEZIER_BASE_H
