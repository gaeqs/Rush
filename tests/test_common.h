//
// Created by gaelr on 07/09/2023.
//

#ifndef RUSH_TEST_COMMON_H
#define RUSH_TEST_COMMON_H

#include <algorithm>
#include <concepts>

#include <rush/rush.h>

#include <catch2/catch_test_macros.hpp>
#include <catch2/benchmark/catch_benchmark.hpp>

template<std::floating_point F>
inline void requireSimilar(F a, F b, F epsilon = F(0.01)) {
    if (std::abs(a - b) >= epsilon) {
        FAIL(a << " != " << b);
    }
}

template<std::floating_point F>
inline void requireNotSimilar(F a, F b, F epsilon = F(0.01)) {
    if (std::abs(a - b) < epsilon) {
        FAIL(a << " == " << b);
    }
}

template<size_t Size, std::floating_point F>
inline void requireSimilar(rush::Vec<Size, F> a, rush::Vec<Size, F> b,
                           F epsilon = F(0.01)) {
    for (int i = 0; i < Size; ++i) {
        if (std::abs(a[i] - b[i]) >= epsilon) {
            FAIL(a << " != " << b);
        }
    }
}

template<size_t Size, std::floating_point F>
inline void requireNotSimilar(rush::Vec<Size, F> a, rush::Vec<Size, F> b,
                              F epsilon = F(0.01)) {
    for (int i = 0; i < Size; ++i) {
        if (std::abs(a[i] - b[i]) >= epsilon) {
            return;
        }
    }
    FAIL(a << " == " << b);
}

template<size_t C, size_t R, std::floating_point F>
inline void requireSimilar(rush::Mat<C, R, F> a, rush::Mat<C, R, F> b,
                           F epsilon = F(0.01)) {
    for (int c = 0; c < C; ++c) {
        for (int r = 0; r < R; ++r) {
            if (std::abs(a(c, r) - b(c, r)) >= epsilon) {
                FAIL(a << "\n!=\n" << b);
            }
        }
    }
}

template<size_t C, size_t R, std::floating_point F>
inline void requireNotSimilar(rush::Mat<C, R, F> a, rush::Mat<C, R, F> b,
                              F epsilon = F(0.01)) {
    for (int c = 0; c < C; ++c) {
        for (int r = 0; r < R; ++r) {
            if (std::abs(a(c, r) - b(c, r)) >= epsilon) {
                return;
            }
        }
    }
    FAIL(a << " == " << b);

}

template<std::floating_point F>
inline void requireSimilar(rush::Quat<F> a, rush::Quat<F> b,
                           F epsilon = F(0.01)) {
    if (std::abs(a.s - b.s) >= epsilon) FAIL(a << " != " << b);
    if (std::abs(a.x - b.x) >= epsilon) FAIL(a << " != " << b);
    if (std::abs(a.y - b.y) >= epsilon) FAIL(a << " != " << b);
    if (std::abs(a.z - b.z) >= epsilon) FAIL(a << " != " << b);
}

template<std::floating_point F>
inline void requireNotSimilar(rush::Quat<F> a, rush::Quat<F> b,
                              F epsilon = F(0.01)) {
    if (std::abs(a.s - b.s) >= epsilon) return;
    if (std::abs(a.x - b.x) >= epsilon) return;
    if (std::abs(a.y - b.y) >= epsilon) return;
    if (std::abs(a.z - b.z) >= epsilon) return;
    FAIL(a << " == " << b);
}


#endif //RUSH_TEST_COMMON_H
