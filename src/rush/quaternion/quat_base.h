//
// Created by gaelr on 12/08/2023.
//

#ifndef RUSH_QUAT_BASE_H
#define RUSH_QUAT_BASE_H

#include <rush/vector/vec.h>
#include <rush/matrix/mat.h>

namespace rush {

    template<typename Type>
    struct Quat {

        using Self = Quat<Type>;

        Type d, a, b, c;

        Quat();

        Quat(Type d, Type a, Type b, Type c);

        Quat(Type angle, Vec<3, Type> direction);

        inline Type squaredLength() requires HasAdd<Type> && HasMul<Type>;

        inline Type length() requires HasSquaredRoot<Type>;

        inline Quat normalize() requires HasDiv<Type>;

        inline void normalized() requires HasDiv<Type>;

        // REGION STATIC

        inline static Self angleAxis(Type angle, const Vec<3, Type>& axis);

        inline static Self euler(const Vec<3, Type>& angles);

        inline static Self
        fromTo(const Vec<3, Type>& from, const Vec<3, Type>& to);

        // ENDREGION

    };

}

#include <rush/quaternion/quat_impl.h>

#endif //RUSH_QUAT_BASE_H
