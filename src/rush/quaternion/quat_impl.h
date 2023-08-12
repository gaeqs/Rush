//
// Created by gaelr on 12/08/2023.
//

#ifndef RUSH_QUAT_IMPL_H
#define RUSH_QUAT_IMPL_H

#include <rush/quaternion/quat_base.h>

namespace rush {

    template<typename Type>
    Quat<Type>::Quat() : d(Type(0)), a(Type(1)), b(Type(0)), c(Type(0)) {
    }

    template<typename Type>
    Quat<Type>::Quat(Type d_, Type a_, Type b_, Type c_) :
            d(d_), a(a_), b(b_), c(c_) {
    }

    template<typename Type>
    Quat<Type>::Quat(Type angle, Vec<3, Type> direction) {

    }

    template<typename Type>
    Type Quat<Type>::squaredLength() requires HasAdd<Type> && HasMul<Type> {
        return d * d + a * a + b * b + c * c;
    }

    template<typename Type>
    Type Quat<Type>::length() requires HasSquaredRoot<Type> {
        return std::sqrt(squaredLength());
    }

    template<typename Type>
    Quat<Type> Quat<Type>::normalize() requires HasDiv<Type> {
        Type l = length();
        return {d / l, a / l, b / l, c / l};
    }

    template<typename Type>
    void Quat<Type>::normalized() requires HasDiv<Type> {
        Type l = length();
        d /= l;
        a /= l;
        b /= l;
        c /= l;
    }

    template<typename Type>
    Quat<Type>::Self
    Quat<Type>::angleAxis(Type angle, const Vec<3, Type>& axis) {
        Type sin = std::sin(angle / Type(2));
        Type d = std::cos(angle / Type(2));
        Type a = axis.x() * sin;
        Type b = axis.y() * sin;
        Type c = axis.z() * sin;
        return {d, a, b, c};
    }

    template<typename Type>
    Quat<Type>::Self Quat<Type>::euler(const Vec<3, Type>& angles) {
        Vec<3, Type> c = rush::cos(angles);
        Vec<3, Type> s = rush::sin(angles);

        return {
                c.x() * c.y() * c.z() + s.x() * s.y() * s.z(),
                s.x() * c.y() * c.z() - c.x() * s.y() * s.z(),
                c.x() * s.y() * c.z() + s.x() * c.y() * s.z(),
                c.x() * c.y() * s.z() - s.x() * s.y() * c.z()
        };
    }

    template<typename Type>
    Quat<Type>::Self
    Quat<Type>::fromTo(const Vec<3, Type>& from, const Vec<3, Type>& to) {
        Type uv = std::sqrt(from.squaredLength() * to.squaredLength());
        Type real = uv + from % to;

        Vec<3, Type> u;
        if (real < Type(1.e-6f) * uv) {
            real = Type(0);
            if (std::abs(from.x()) > std::abs(from.z())) {
                u = {-from.y(), from.x(), Type(0)};
            } else {
                u = {Type(0), -from.z(), from.y()};
            }
        } else {
            u = from ^ to;
        }
        return Quat<Type>(real, u.x(), u.y(), u.z()).normalize();
    }
}

#endif //RUSH_QUAT_IMPL_H
