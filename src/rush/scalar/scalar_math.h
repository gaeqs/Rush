//
// Created by gaelr on 22/09/2023.
//

#ifndef NEON_SCALAR_ALGORITHMS_BASE_H
#define NEON_SCALAR_ALGORITHMS_BASE_H

#include <numbers>

namespace rush {

    template<typename T>
    constexpr T degrees(T other) {
        return other * T(180) / std::numbers::pi_v<T>;
    }

    template<typename T>
    constexpr T radians(T other) {
        return other * std::numbers::pi_v<T> / T(180);
    }

    template<typename T>
    requires std::is_integral_v<T>
    constexpr T factorial(T v) {
        if (v < T(1)) return T(0);

        T result = T(1);
        while (v > T(1)) {
            result *= v;
            v--;
        }

        return result;
    }

    template<typename T>
    requires std::is_integral_v<T>
    constexpr T binomial(T n, T k) {
        if(k > n) return T(0);
        T top = T(1);
        T bottom = T(1);
        for (T i = T(1); i <= k; ++i) {
            top *= (n - k + i);
            bottom *= i;
        }
        return top / bottom;
    }

}

#endif //NEON_SCALAR_ALGORITHMS_BASE_H
