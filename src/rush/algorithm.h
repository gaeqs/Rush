//
// Created by gaelr on 12/08/2023.
//

#ifndef RUSH_QUALITY_H
#define RUSH_QUALITY_H

#if defined(_M_IX86) || defined(_M_X64)

#include <immintrin.h>

constexpr bool IntrinsicsAvailable = true;

#else

constexpr bool IntrinsicsAvailable = false;

#endif

namespace rush {

    enum class Precision {
        High,
        Low
    };

    enum class Mode {
        General,
        Intrinsics
    };

    struct Algorithm {
        Precision precision = Precision::High;
        Mode mode = Mode::Intrinsics;

        constexpr bool useIntrinsics() const {
            return mode == Mode::Intrinsics && IntrinsicsAvailable;
        }
    };

    constexpr Algorithm HIGH_INTRINSICS = Algorithm();
    constexpr Algorithm HIGH_GENERAL = Algorithm(Precision::High,
                                                 Mode::General);
    constexpr Algorithm LOW_INTRINSICS = Algorithm(Precision::Low);
    constexpr Algorithm LOW_GENERAL = Algorithm(Precision::Low, Mode::General);

}

#endif //RUSH_QUALITY_H
