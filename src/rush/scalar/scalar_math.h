//
// Created by gaelr on 22/09/2023.
//

#ifndef NEON_SCALAR_ALGORITHMS_BASE_H
#define NEON_SCALAR_ALGORITHMS_BASE_H

#include <numbers>

namespace rush {

    template<typename T>
    T degrees(T other) {
        return other * T(180) / std::numbers::pi_v<T>;
    }

    template<typename T>
    T radians(T other) {
        return other * std::numbers::pi_v<T> / T(180);
    }

}

#endif //NEON_SCALAR_ALGORITHMS_BASE_H
