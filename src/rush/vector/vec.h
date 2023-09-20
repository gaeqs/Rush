//
// Created by gaelr on 31/07/2023.
//

#ifndef RUSH_VEC_H
#define RUSH_VEC_H

#include <rush/vector/vec_base.h>
#include <rush/vector/vec_extra.h>
#include <rush/vector/vec_ref.h>
#include <rush/vector/vec_math.h>

namespace rush {
    using Vec1f = rush::Vec<1, float>;
    using Vec2f = rush::Vec<2, float>;
    using Vec3f = rush::Vec<3, float>;
    using Vec4f = rush::Vec<4, float>;

    using Vec1d = rush::Vec<1, double>;
    using Vec2d = rush::Vec<2, double>;
    using Vec3d = rush::Vec<3, double>;
    using Vec4d = rush::Vec<4, double>;

    using Vec1i = rush::Vec<1, int32_t>;
    using Vec2i = rush::Vec<2, int32_t>;
    using Vec3i = rush::Vec<3, int32_t>;
    using Vec4i = rush::Vec<4, int32_t>;

    using Vec1l = rush::Vec<1, int64_t>;
    using Vec2l = rush::Vec<2, int64_t>;
    using Vec3l = rush::Vec<3, int64_t>;
    using Vec4l = rush::Vec<4, int64_t>;
}


#endif //RUSH_VEC_H
