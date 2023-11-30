//
// Created by gaelr on 31/07/2023.
//

#ifndef RUSH_CONCEPTS_H
#define RUSH_CONCEPTS_H

#include <cmath>

namespace rush {

    template<typename Type>
    concept HasAdd = requires(Type a, Type b) {
        a + b;
    };

    template<typename Type>
    concept HasSub = requires(Type a, Type b) {
        a - b;
    };

    template<typename Type>
    concept HasMul = requires(Type a, Type b) {
        a * b;
    };

    template<typename Type>
    concept HasDiv = requires(Type a, Type b) {
        a / b;
    };

    template<typename Type>
    concept HasMod = requires(Type a, Type b) {
        a % b;
    };

    template<typename Type>
    concept HasShl = requires(Type a, Type b) {
        a << b;
    };

    template<typename Type>
    concept HasShr = requires(Type a, Type b) {
        a >> b;
    };

    template<typename Type>
    concept HasBitAnd = requires(Type a, Type b) {
        a & b;
    };

    template<typename Type>
    concept HasBitOr = requires(Type a, Type b) {
        a | b;
    };

    template<typename Type>
    concept HasBitXor = requires(Type a, Type b) {
        a ^ b;
    };

    template<typename Type>
    concept HasBitNot = requires(Type a) {
        ~a;
    };

    template<typename Type>
    concept HasAnd = requires(Type a, Type b) {
        a && b;
    };

    template<typename Type>
    concept HasOr = requires(Type a, Type b) {
        a || b;
    };

    template<typename Type>
    concept HasNot = requires(Type a) {
        !a;
    };

    template<typename Type>
    concept HasSquaredRoot = requires(Type a) {
        std::sqrt(a);
    };

    template<typename Type>
    constexpr void assertAddition() {
        static_assert(HasAdd<Type>,
                      "Type doesn't have an addition operator defined");

    }

    template<typename Type>
    constexpr void assertSubtraction() {
        static_assert(HasSub<Type>,
                      "Type doesn't have a subtraction operator defined");

    }

    template<typename Type>
    constexpr void assertMultiplication() {
        static_assert(HasMul<Type>,
                      "Type doesn't have a multiplication operator defined");

    }

    template<typename Type>
    constexpr void assertDivision() {
        static_assert(HasDiv<Type>,
                      "Type doesn't have a division operator defined");
    }
}

#endif //RUSH_CONCEPTS_H