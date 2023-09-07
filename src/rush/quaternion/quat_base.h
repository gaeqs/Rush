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

        Type s, x, y, z;

        Quat();

        Quat(Type d, Type a, Type b, Type c);

        Quat(Type angle, Vec<3, Type> direction);

        // REGION OPERATORS

        // UNARY

        inline Type squaredLength() const requires HasAdd<Type> && HasMul<Type>;

        inline Type length() const requires HasSquaredRoot<Type>;

        inline Quat normalized() const requires HasDiv<Type>;

        inline void normalize() requires HasDiv<Type>;

        inline Quat conjugated() const;

        inline void conjugate();

        inline Quat inverse() const;

        inline void invert();

        // QUAT - SCALAR

        inline Quat operator+(const Type& o) const requires HasAdd<Type>;
        inline Quat operator-(const Type& o) const requires HasAdd<Type>;
        inline Quat operator*(const Type& o) const requires HasAdd<Type>;
        inline Quat operator/(const Type& o) const requires HasAdd<Type>;

        // QUAT - QUAT

        inline Quat operator+(const Quat& o) const requires HasAdd<Type>;
        inline Quat operator-(const Quat& o) const requires HasSub<Type>;
        inline Quat operator*(const Quat& o) const requires
        HasAdd<Type> && HasSub<Type> && HasMul<Type>;

        // ENDREGION

        // REGION STATIC CREATION

        inline static Quat angleAxis(Type angle, const Vec<3, Type>& axis);

        inline static Quat euler(const Vec<3, Type>& angles);

        inline static Quat
        fromTo(const Vec<3, Type>& from, const Vec<3, Type>& to);

        // ENDREGION

    };

}

#include <rush/quaternion/quat_impl.h>

#endif //RUSH_QUAT_BASE_H
