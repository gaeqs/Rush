//
// Created by gaelr on 12/08/2023.
//

#ifndef RUSH_QUALITY_H
#define RUSH_QUALITY_H

#if defined(_M_IX86) || defined(_M_X64) || defined(__SSE__)

#include <immintrin.h>

#define RUSH_INTRINSICS
namespace rush {
    constexpr bool IntrinsicsAvailable = true;
}

#else

namespace rush {
    constexpr bool IntrinsicsAvailable = false;
}


#endif

namespace rush {

    constexpr float EPSILON = 0.0001f;

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

    /**
     * Enum used to specify whether an algorithm should use
     * a left-handed or right-handed coordinate system.
     */
    enum class Hand {
        Right,
        Left
    };

    /**
     * Enum used to specify the projection matrix format.
     */
    enum class ProjectionFormat {
        OpenGL,
        Vulkan,
        DirectX
    };
}

#endif //RUSH_QUALITY_H
