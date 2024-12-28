//
// Created by gaelr on 12/08/2023.
//

#ifndef RUSH_QUAT_IMPL_H
#define RUSH_QUAT_IMPL_H

#include <numbers>

#include <rush/quaternion/quat_base.h>

namespace rush {
    template<typename Type>
    Quat<Type>::Quat() : s(Type(1)), x(Type(0)), y(Type(0)), z(Type(0)) {}

    template<typename Type>
    Quat<Type>::Quat(Type d_, Type a_, Type b_, Type c_) : s(d_), x(a_), y(b_),
                                                           z(c_) {}

    template<typename Type>
    Type& Quat<Type>::operator[](size_t index) {
        return *(&s + index);
    }

    template<typename Type>
    const Type& Quat<Type>::operator[](size_t index) const {
        return *(&s + index);
    }

    template<typename Type>
    Quat<Type>& Quat<Type>::operator+() {
        return *this;
    }

    template<typename Type>
    const Quat<Type>& Quat<Type>::operator+() const {
        return *this;
    }

    template<typename Type>
    Quat<Type> Quat<Type>::operator-() const {
        return {-s, -x, -y, -z};
    }

    template<typename Type>
    Type
    Quat<Type>::squaredLength() const requires HasAdd<Type> && HasMul<Type> {
        return s * s + x * x + y * y + z * z;
    }

    template<typename Type>
    template<typename Return>
    Return Quat<Type>::length() const requires
        std::is_convertible_v<Type, Return> && HasSquaredRoot<Type> {
        return static_cast<Return>(std::sqrt(squaredLength()));
    }

    template<typename Type>
    template<typename Return, Algorithm A>
    Return Quat<Type>::inverseLength() const requires (
        std::is_convertible_v<Type, Return> && HasSquaredRoot<Type>) {
        if constexpr (A.precision == Precision::High) {
            return 1.0f / std::sqrt(squaredLength());
        }

        if constexpr (std::is_same_v<Return, float>) {
            auto v = static_cast<float>(squaredLength());

#ifdef RUSH_INTRINSICS
            if constexpr (A.useIntrinsics()) {
                return _mm_cvtss_f32(_mm_rsqrt_ss(_mm_set_ss(v)));
            }
#endif

            float x2 = v * 0.5f;
            // what the fuck?
            uint32_t i = 0x5f3759df - (std::bit_cast<uint32_t>(v) >> 1);
            v = std::bit_cast<float>(i);
            return v * (1.5f - x2 * v * v);
        } else if constexpr (std::is_same_v<Return, double>) {
            auto v = static_cast<double>(squaredLength());
            double x2 = v * 0.5f;
            // what the fuck?
            uint64_t i = 0x5fe6eb50c7b537a9 - (std::bit_cast<uint64_t>(v) >> 1);
            v = std::bit_cast<double>(i);
            return v * (1.5f - x2 * v * v);
        }

        return 1.0f / std::sqrt(squaredLength());
    }

    template<typename Type>
    bool Quat<Type>::isNormalized(Type epsilon) const {
        return std::abs(squaredLength() - (Type)1) < epsilon;
    }

    template<typename Type>
    template<typename Return, Algorithm A>
    Quat<Return> Quat<Type>::normalized() const requires HasDiv<Type> {
        Return l = inverseLength<Return, A>();
        return {s * l, x * l, y * l, z * l};
    }

    template<typename Type>
    Quat<Type> Quat<Type>::conjugate() const {
        return {s, -x, -y, -z};
    }

    template<typename Type>
    Quat<Type> Quat<Type>::inverse() const {
        return conjugate() / squaredLength();
    }

    template<typename Type>
    Type Quat<Type>::pitch() const {
        Type rY = Type(2) * (y * z + s * x);
        Type rX = s * s - x * x - y * y + z * z;
        if (rY < EPSILON && rY > -EPSILON &&
            rX < EPSILON && rY > -EPSILON)
            return Type(2) * std::atan2(x, s);
        return std::atan2(rY, rX);
    }

    template<typename Type>
    Type Quat<Type>::yaw() const {
        Type t = std::clamp(Type(-2) * (x * z - s * y), Type(-1), Type(1));
        return std::asin(t);
    }

    template<typename Type>
    Type Quat<Type>::roll() const {
        Type rY = Type(2) * (x * y + s * z);
        Type rX = s * s + x * x - y * y - z * z;
        if (rY < EPSILON && rY > -EPSILON &&
            rX < EPSILON && rY > -EPSILON)
            return Type(0);
        return std::atan2(rY, rX);
    }

    template<typename Type>
    template<typename Alloc>
    Vec<3, Type, Alloc> Quat<Type>::euler() const {
        return {pitch(), yaw(), roll()};
    }

    template<typename Type>
    template<typename Alloc>
    Vec<3, Type, Alloc> Quat<Type>::directionalPart() const {
        return {x, y, z};
    }

    template<typename Type>
    template<typename ARep, typename Alloc>
    Mat<3, 3, Type, ARep, Alloc> Quat<Type>::rotationMatrix3() const {
        Mat<3, 3, Type, ARep, Alloc> r;

        Type o = Type(1);
        Type t = Type(2);

        r(0, 0) = o - t * (y * y + z * z);
        r(0, 1) = t * (x * y + s * z);
        r(0, 2) = t * (x * z - s * y);
        r(1, 0) = t * (x * y - s * z);
        r(1, 1) = o - t * (x * x + z * z);
        r(1, 2) = t * (y * z + s * x);
        r(2, 0) = t * (x * z + s * y);
        r(2, 1) = t * (y * z - s * x);
        r(2, 2) = o - t * (x * x + y * y);

        return r;
    }

    template<typename Type>
    template<typename ARep, typename Alloc>
    Mat<4, 4, Type, ARep, Alloc> Quat<Type>::rotationMatrix4() const {
        Mat<4, 4, Type, ARep, Alloc> r(Type(1.0));

        Type o = Type(1);
        Type t = Type(2);

        r(0, 0) = o - t * (y * y + z * z);
        r(0, 1) = t * (x * y + s * z);
        r(0, 2) = t * (x * z - s * y);
        r(1, 0) = t * (x * y - s * z);
        r(1, 1) = o - t * (x * x + z * z);
        r(1, 2) = t * (y * z + s * x);
        r(2, 0) = t * (x * z + s * y);
        r(2, 1) = t * (y * z - s * x);
        r(2, 2) = o - t * (x * x + y * y);

        return r;
    }

    template<typename Type>
    template<typename Alloc>
    Vec<4, Type, Alloc> Quat<Type>::toVec() const {
        return Vec<4, Type, Alloc>(s, x, y, z);
    }

    template<typename Type>
    template<typename To>
    Quat<To> Quat<Type>::cast() const {
        return Quat<To>(
            static_cast<To>(s),
            static_cast<To>(x),
            static_cast<To>(y),
            static_cast<To>(z)
        );
    }

    template<typename Type>
    Quat<Type>
    Quat<Type>::operator+(const Type& o) const requires HasAdd<Type> {
        return {s + o, x + o, y + o, z + o};
    }

    template<typename Type>
    Quat<Type>
    Quat<Type>::operator-(const Type& o) const requires HasSub<Type> {
        return {s - o, x - o, y - o, z - o};
    }

    template<typename Type>
    Quat<Type>
    Quat<Type>::operator*(const Type& o) const requires HasMul<Type> {
        return {s * o, x * o, y * o, z * o};
    }

    template<typename Type>
    Quat<Type>
    Quat<Type>::operator/(const Type& o) const requires HasDiv<Type> {
        return {s / o, x / o, y / o, z / o};
    }

    template<typename Type>
    Type Quat<Type>::dot(const Quat& o) const {
        return s * o.s + x * o.x + y * o.y + z * o.z;
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
    bool Quat<Type>::operator==(const Quat& o) const {
        return s == o.s && x == o.x && y == o.y && z == o.z;
    }

    template<typename Type>
    bool Quat<Type>::operator!=(const Quat& o) const {
        return s != o.s || x != o.x || y != o.y || z != o.z;
    }

    template<typename Type>
    Vec<3, Type> Quat<Type>::operator*(const Vec<3, Type>& o) const {
        Vec<3, Type> u = directionalPart();
        return Type(2) * u.dot(o) * u
               + (s * s - u.squaredLength()) * o
               + Type(2) * s * u.cross(o);
    }

    template<typename Type>
    Quat<Type> Quat<Type>::lerp(const Quat<Type>& o, Type a) const {
        return *this * (Type(1) - a) + (o * a);
    }

    template<typename Type>
    Quat<Type> Quat<Type>::slerp(const Quat<Type>& o, Type a) const {
        constexpr Type EPSILON = Type(0.99);

        auto q = o;
        Type cos = dot(o);
        if (cos < Type(0)) {
            // The interpolation will take the long path.
            // Invert the quaternion sign.
            q = -o;
            cos = -cos;
        }

        if (cos > EPSILON) {
            // Use linear!
            return lerp(q, a);
        }

        // The slerp itself.
        Type angle = std::acos(cos);
        auto first = *this * std::sin((Type(1) - a) * angle);
        auto second = q * std::sin(a * angle);
        return (first + second) / std::sin(angle);
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
        Vec<3, Type> c = rush::cos(angles / Type(2));
        Vec<3, Type> s = rush::sin(angles / Type(2));

        return Quat{
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
        Type real = uv + from.dot(to);

        Vec<3, Type> u;
        if (real < Type(1.e-6f) * uv) {
            real = Type(0);
            if (std::abs(from.x()) > std::abs(from.z())) {
                u = {-from.y(), from.x(), Type(0)};
            } else {
                u = {Type(0), -from.z(), from.y()};
            }
        } else {
            u = from.cross(to);
        }
        return Quat(real, u.x(), u.y(), u.z()).normalized();
    }

    template<typename Type>
    Quat<Type> Quat<Type>::lookAt(Vec<3, Type> direction) {
        static const Vec<3, Type> F = {Type(0), Type(0), Type(-1)};
        static const Vec<3, Type> UP = {Type(0), Type(1), Type(0)};
        Type dot = F.dot(direction);

        if (dot < -Type(0.9999)) {
            return Quat::angleAxis(std::numbers::pi_v<Type>, UP);
        }
        if (dot > Type(0.9999)) {
            return Quat();
        }

        Type angle = std::acos(dot);
        Vec<3, Type> axis = F.cross(direction).normalized();

        return Quat::angleAxis(angle, axis);
    }

    template<typename Type>
    Quat<Type> Quat<Type>::fromRotationMatrix(const Mat<3, 3, Type>& rot) {
        Quat q;
        auto trace = rot(0, 0) + rot(1, 1) + rot(2, 2);
        if (trace > 0.0f) {
            auto s = 2.0f * std::sqrt(trace + 1.0f);
            q.s = s / 4.0f;
            q.x = (rot(1, 2) - rot(2, 1)) / s;
            q.y = (rot(2, 0) - rot(0, 2)) / s;
            q.z = (rot(0, 1) - rot(1, 0)) / s;
        } else if (rot(0, 0) > rot(1, 1) && rot(0, 0) > rot(2, 2)) {
            auto s = 2.0f * std::sqrt(1.0f + rot(0, 0) - rot(1, 1) - rot(2, 2));
            q.s = (rot(1, 2) - rot(2, 1)) / s;
            q.x = s / 4.0f;
            q.y = (rot(0, 1) + rot(1, 0)) / s;
            q.z = (rot(0, 2) + rot(2, 0)) / s;
        } else if (rot(1, 1) > rot(2, 2)) {
            auto s = 2.0f * std::sqrt(1.0f + rot(1, 1) - rot(0, 0) - rot(2, 2));
            q.s = (rot(2, 0) - rot(0, 2)) / s;
            q.x = (rot(0, 1) + rot(1, 0)) / s;
            q.y = s / 4.0f;
            q.z = (rot(1, 2) + rot(2, 1)) / s;
        } else {
            auto s = 2.0f * std::sqrt(1.0f + rot(2, 2) - rot(0, 0) - rot(1, 1));
            q.s = (rot(1, 0) - rot(0, 1)) / s;
            q.x = (rot(0, 2) + rot(2, 0)) / s;
            q.y = (rot(1, 2) + rot(2, 1)) / s;
            q.z = s / 4.0f;
        }

        return q;
    }

    template<typename Type>
    auto Quat<Type>::begin() {
        return &s;
    }

    template<typename Type>
    auto Quat<Type>::end() {
        return &z + 1;
    }

    template<typename Type>
    auto Quat<Type>::cbegin() const {
        return &s;
    }

    template<typename Type>
    auto Quat<Type>::cend() const {
        return &z + 1;
    }

    template<typename Type>
    auto Quat<Type>::rbegin() {
        return std::reverse_iterator<double*>(&z + 1);
    }

    template<typename Type>
    auto Quat<Type>::rend() {
        return std::reverse_iterator<double*>(&s);
    }

    template<typename Type>
    auto Quat<Type>::crbegin() const {
        return std::reverse_iterator<const double*>(&z + 1);
    }

    template<typename Type>
    auto Quat<Type>::crend() const {
        return std::reverse_iterator<const double*>(&s);
    }
}

#endif //RUSH_QUAT_IMPL_H
