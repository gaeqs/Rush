//
// Created by gaelr on 12/08/2023.
//

#ifndef RUSH_QUAT_IMPL_H
#define RUSH_QUAT_IMPL_H

#include <rush/quaternion/quat_base.h>

namespace rush {

    template<typename Type>
    Quat<Type>::Quat() : s(Type(0)), x(Type(1)), y(Type(0)), z(Type(0)) {
    }

    template<typename Type>
    Quat<Type>::Quat(Type d_, Type a_, Type b_, Type c_) :
            s(d_), x(a_), y(b_), z(c_) {
    }

    template<typename Type>
    Quat<Type>::Quat(Type angle, Vec<3, Type> direction) {

    }

    template<typename Type>
    Type
    Quat<Type>::squaredLength() const requires HasAdd<Type> && HasMul<Type> {
        return s * s + x * x + y * y + z * z;
    }

    template<typename Type>
    Type Quat<Type>::length() const requires HasSquaredRoot<Type> {
        return std::sqrt(squaredLength());
    }

    template<typename Type>
    Quat<Type> Quat<Type>::normalized() const requires HasDiv<Type> {
        Type l = length();
        return {s / l, x / l, y / l, z / l};
    }

    template<typename Type>
    void Quat<Type>::normalize() requires HasDiv<Type> {
        Type l = length();
        s /= l;
        x /= l;
        y /= l;
        z /= l;
    }

    template<typename Type>
    Quat<Type> Quat<Type>::conjugated() const {
        return {s, -x, -y, -z};
    }

    template<typename Type>
    void Quat<Type>::conjugate() {
        x = -x;
        y = -y;
        z = -z;
    }

    template<typename Type>
    Quat<Type> Quat<Type>::inverse() const {
        return conjugated() / squaredLength();
    }

    template<typename Type>
    void Quat<Type>::invert() {
        Type sl = squaredLength();
        s = s / sl;
        x = -x / sl;
        y = -y / sl;
        z = -z / sl;
    }

    template<typename Type>
    Quat<Type>
    Quat<Type>::operator+(const Type& o) const requires HasAdd<Type> {
        return {s + o, x + o, y + o, z + o};
    }

    template<typename Type>
    Quat<Type>
    Quat<Type>::operator-(const Type& o) const requires HasAdd<Type> {
        return {s - o, x - o, y - o, z - o};
    }

    template<typename Type>
    Quat<Type>
    Quat<Type>::operator*(const Type& o) const requires HasAdd<Type> {
        return {s * o, x * o, y * o, z * o};
    }

    template<typename Type>
    Quat<Type>
    Quat<Type>::operator/(const Type& o) const requires HasAdd<Type> {
        return {s / o, x / o, y / o, z / o};
    }


    template<typename Type>
    Quat<Type>
    Quat<Type>::operator+(const Quat& o) const requires HasAdd<Type> {
        return {s + o.s, x + o.x, y + o.y, z + o.z};
    }

    template<typename Type>
    Quat<Type>
    Quat<Type>::operator-(const Quat& o) const requires HasSub<Type> {
        return {s - o.s, x - o.x, y - o.y, z - o.z};
    }

    template<typename Type>
    Quat<Type> Quat<Type>::operator*(const Quat& o) const requires
    HasAdd<Type> && HasSub<Type> && HasMul<Type> {
        Quat<Type> result;
        result.s = s * o.s - x * o.x - y * o.y - z * o.z;
        result.x = s * o.x + x * o.s + y * o.z - z * o.y;
        result.y = s * o.y - x * o.z + y * o.s + z * o.x;
        result.z = s * o.z + x * o.y - y * o.x + z * o.s;
        return result;
    }

    template<typename Type>
    Quat<Type>
    Quat<Type>::angleAxis(Type angle, const Vec<3, Type>& axis) {
        Type sin = std::sin(angle / Type(2));
        Type d = std::cos(angle / Type(2));
        Type a = axis.x() * sin;
        Type b = axis.y() * sin;
        Type c = axis.z() * sin;
        return {d, a, b, c};
    }

    template<typename Type>
    Quat<Type> Quat<Type>::euler(const Vec<3, Type>& angles) {
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
    Quat<Type>
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
