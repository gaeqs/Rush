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

        /**
         * Creates a quaternion with an identity rotation: (0, {1, 0, 0}).
         */
        Quat();

        /**
         * Creates a quaternion with the given raw parameters.
         * @param s the scalar value.
         * @param x the x-axis value.
         * @param y the y-axis value.
         * @param z the z-axis value.
         */
        Quat(Type s, Type x, Type y, Type z);

        // REGION ACCESSORS

        /**
         * Returns a reference to the elemento located at
         * the given index.
         * <p>
         * This method doesn't check if the index is invalid.
         *
         * @param index the index.
         * @return the element.
         */
        inline Type& operator[](size_t index);

        /**
         * Returns a reference to the elemento located at
         * the given index.
         * <p>
         * This method doesn't check if the index is invalid.
         *
         * @param index the index.
         * @return the element.
         */
        inline const Type& operator[](size_t index) const;

        // ENDREGION

        // REGION OPERATORS

        // UNARY

        inline Quat& operator+();
        inline const Quat& operator+() const;
        inline Quat operator-() const;

        /**
         * Returns the squared length (or squared modulus) of this quaternion.
         * The squared length is defined the as the sum of all
         * entries of this quaternion (including the scalar)
         * multiplied by themselves.
         *
         * @return the squared length.
         */
        inline Type squaredLength() const requires HasAdd<Type> && HasMul<Type>;

        /**
         * Returns the length (or modulus) of this quaternion.
         * The length is defined the as the squared root of the sum of all
         * entries of this quaternion (including the scalar)
         * multiplied by themselves.
         *
         * @return the length.
         */
        template<typename Return = Type>
        inline Return length() const requires
        std::is_convertible_v<Type, Return> && HasSquaredRoot<Type>;

        /**
         * Returns the inverse of the length (or modulus) of this quaternion.
         * The length is defined the as the squared root of the sum of all
         * entries of this quaternion multiplied by themselves.
         * <p>
         * You can select the precision of the algorithm.
         * A high precision algorithm will provide a high precision
         * result in exchange of more computational cost.
         * A low precision algorithm will be faster, but it may provide
         * poorer results.
         * <p>
         * You can also select whether the algorithm will use pure C++ code
         * or it will try to use intrinsic processor instructions.
         *
         * @return the length.
         *
         */
        template<typename Return = Type, Algorithm Algorithm = Algorithm()>
        Return inverseLength() const requires (
        std::is_convertible_v<Type, Return> && HasSquaredRoot<Type>);

        /**
         * Returns a normalized version of this vector.
         * <p>
         * You can select the precision of the algorithm.
         * A high precision algorithm will provide a high precision
         * result in exchange of more computational cost.
         * A low precision algorithm will be faster, but it may provide
         * poorer results.
         * <p>
         * You can also select whether the algorithm will use pure C++ code
         * or it will try to use intrinsic processor instructions.
         *
         * @return the normalized vector.
         *
         */
        template<typename Return = Type, Algorithm Algorithm = Algorithm()>
        inline Quat<Return> normalized() const requires HasDiv<Type>;

        /**
         * Returns the conjugate of this quaternion.
         * <p>
         * The conjugate of a quaternion q (s, x, y, z) is
         * defined as q* = (s, -x, -y, -z).
         * When using quaternions as rotations, the conjugate
         * represents the inverse rotation.
         *
         * @return the conjugate.
         */
        inline Quat conjugate() const;

        /**
         * Returns the inverse of this quaternion.
         * <p>
         * This is not the inverse rotation! Use the conjugate instead.
         * @return the inverse.
         */
        inline Quat inverse() const;

        /**
         * Returns the pitch of the rotation represented by this
         * quaternion.
         * @return the pitch in radians.
         */
        Type pitch() const;

        /**
         * Returns the yaw of the rotation represented by this
         * quaternion.
         * @return the yaw in radians.
         */
        Type yaw() const;

        /**
         * Returns the roll angle of the rotation represented by this
         * quaternion.
         * @return the roll in radians.
         */
        Type roll() const;

        /**
         * Returns the euler angles of the rotation represented by this
         * quaternion (pitch, yaw and roll).
         * @tparam Alloc the allocator to use in the vector containing the angles.
         * @return the angles in radians.
         */
        template<typename Alloc = StaticAllocator>
        inline Vec<3, Type, Alloc> euler() const;

        /**
         * Returns the directional part of this quaternion
         * (x, y and z components).
         * @tparam Alloc the allocator of the vector.
         * @return the directional part.
         */
        template<typename Alloc = StaticAllocator>
        inline Vec<3, Type, Alloc> directionalPart() const;

        /**
         * Returns the rotation matrix that applies the
         * rotation represented by this quaternion.
         * @tparam Alloc the allocator of the matrix.
         * @return the rotation matrix.
         */
        template<typename Alloc = StaticAllocator>
        Mat<3, 3, Type, Alloc> rotationMatrix3() const;

        /**
         * Returns the rotation matrix that applies the
         * rotation represented by this quaternion.
         * @tparam Alloc the allocator of the matrix.
         * @return the rotation matrix.
         */
        template<typename Alloc = StaticAllocator>
        Mat<4, 4, Type, Alloc> rotationMatrix4() const;

        /**
         * Returns a vector containing all the data of this quaternion.
         * @tparam Alloc the allocator of the vector.
         * @return the vector.
         */
        template<typename Alloc = StaticAllocator>
        Vec<4, Type, Alloc> toVec() const;

        // QUAT - SCALAR

        inline Quat operator+(const Type& o) const requires HasAdd<Type>;
        inline Quat operator-(const Type& o) const requires HasSub<Type>;
        inline Quat operator*(const Type& o) const requires HasMul<Type>;
        inline Quat operator/(const Type& o) const requires HasDiv<Type>;

        // QUAT - QUAT

        inline Type dot(const Quat& o) const;

        inline Quat operator+(const Quat& o) const requires HasAdd<Type>;
        inline Quat operator-(const Quat& o) const requires HasSub<Type>;
        inline Quat operator*(const Quat& o) const requires
        HasAdd<Type> && HasSub<Type> && HasMul<Type>;

        inline bool operator==(const Quat& o) const;
        inline bool operator!=(const Quat& o) const;

        // QUAT - VEC

        inline Vec<3, Type> operator*(const Vec<3, Type>& o) const;

        // LERP

        Quat lerp(const Quat<Type>& o, Type a) const;
        Quat slerp(const Quat<Type>& o, Type a) const;

        // ENDREGION

        // REGION STATIC CREATION

        inline static Quat angleAxis(Type angle, const Vec<3, Type>& axis);

        /**
         * Creates a quaternion that reproduces the rotation
         * defined by the given euler angles (in ZYX order).
         * @param angles the angles (pitch, yaw, roll)
         * @return the quaternion.
         */
        inline static Quat euler(const Vec<3, Type>& angles);

        inline static Quat
        fromTo(const Vec<3, Type>& from, const Vec<3, Type>& to);

        // ENDREGION

        // REGION ITERATOR

        inline auto begin();
        inline auto end();
        inline auto cbegin() const;
        inline auto cend() const;
        inline auto rbegin();
        inline auto rend();
        inline auto crbegin() const;
        inline auto crend() const;

        // ENDREGION

    };

}

#include <rush/quaternion/quat_impl.h>

#endif //RUSH_QUAT_BASE_H
