//
// Created by gaelr on 25/09/23.
//

#include "test_common.h"


TEST_CASE("Bézier segment default", "[bezier]") {
    rush::BezierSegment<2, 3, double> seg;
    for (const auto& item: seg.nodes) {
        requireSimilar(item, rush::Vec3d());
    }
}


TEST_CASE("Bézier segment 2", "[bezier]") {
    rush::BezierSegment<2, 3, double> seg{rush::Vec3d{1.0, 1.0, 1.0},
                                          rush::Vec3d{5.0, 0.0, 1.0}};


    requireSimilar(seg.fetch(0.0), seg.nodes[0]);
    requireSimilar(seg.fetch(1.0), seg.nodes[1]);

    for (double d = 0.0; d < 1.0; d += 0.1) {
        requireSimilar(seg.fetch(d),
                       seg.nodes[1] * d + seg.nodes[0] * (1.0 - d));
    }
}

TEST_CASE("Bézier segment 4", "[bezier]") {
    rush::BezierSegment<4, 3, double> seg{rush::Vec3d{1.0, 1.0, 1.0},
                                          rush::Vec3d{5.0, 0.0, 1.0},
                                          rush::Vec3d{10.0, -2.0, 3.0},
                                          rush::Vec3d{0.0, 1.0, 0.0}};

    requireSimilar(seg.fetch(0.0), seg.nodes[0]);
    requireSimilar(seg.fetch(1.0), seg.nodes[3]);


    rush::Vec3d values[11] = {
            {1.0,   1.0,    1.0},
            {2.214, 0.676,  1.053},
            {3.392, 0.328,  1.184},
            {4.438, -0.008, 1.351},
            {5.256, -0.296, 1.512},
            {5.75,  -0.5,   1.625},
            {5.824, -0.584, 1.648},
            {5.382, -0.512, 1.539},
            {4.328, -0.248, 1.256},
            {2.566, 0.244,  0.757},
            {0.0,   1.0,    0.0}
    };

    double d = 0.0;
    for (const auto& value: values) {
        requireSimilar(seg.fetch(d), value);
        d += 0.1;
    }
}

TEST_CASE("Bézier segment 5", "[bezier]") {
    rush::BezierSegment<5, 3, double> seg{rush::Vec3d{1.0, 1.0, 1.0},
                                          rush::Vec3d{5.0, 0.0, 1.0},
                                          rush::Vec3d{10.0, -2.0, 3.0},
                                          rush::Vec3d{10.0, -10.0, 4.0},
                                          rush::Vec3d{0.0, 1.0, 0.0}};

    requireSimilar(seg.fetch(0.0), seg.nodes[0]);
    requireSimilar(seg.fetch(1.0), seg.nodes[4]);


    rush::Vec3d values[11] = {
            {1.0,   1.0,    1.0},
            {2.636, 0.523,  1.108},
            {4.25,  -0.152, 1.382},
            {5.7,   -1.037, 1.748},
            {6.85,  -2.072, 2.126},
            {7.563, -3.125, 2.438},
            {7.706, -3.992, 2.598},
            {7.148, -4.397, 2.524},
            {5.762, -3.992, 2.126},
            {3.42,  -2.357, 1.316},
            {0.0,   1.0,    0.0}
    };

    double d = 0.0;
    for (const auto& value: values) {
        requireSimilar(seg.fetch(d), value);
        d += 0.1;
    }
}

TEST_CASE("Bézier curve", "[bezier]") {
    rush::BezierSegment<4, 3, double> seg1(
            rush::Vec3d{1.0, 1.0, 1.0},
            rush::Vec3d{5.0, 0.0, 1.0},
            rush::Vec3d{10.0, -2.0, 3.0},
            rush::Vec3d{10.0, -10.0, 4.0}
    );

    auto seg2 = rush::BezierSegment<4, 3, double>::continuousTo(
            seg1,
            rush::Vec3d{3.0, 0.0, 1.0},
            rush::Vec3d{5.0, 2.0, 1.0}
    );

    rush::BezierCurve<2, 4, 3, double> curve(seg1, seg2);

    requireSimilar(seg1.nodes[0], curve.fetch(0.0, true));
    requireSimilar(seg1.nodes[3], curve.fetch(0.5, true));
    requireSimilar(seg2.nodes[3], curve.fetch(1.0, true));
    requireSimilar(curve.fetch(0.499999, true), curve.fetch(0.500001, true));
}

TEST_CASE("Bézier derivative", "[bezier]") {
    rush::BezierSegment<4, 3, double> seg1(
            rush::Vec3d{1.0, 1.0, 1.0},
            rush::Vec3d{5.0, 0.0, 1.0},
            rush::Vec3d{10.0, -2.0, 3.0},
            rush::Vec3d{10.0, -10.0, 4.0}
    );

    auto seg2 = rush::BezierSegment<4, 3, double>::continuousTo(
            seg1,
            rush::Vec3d{3.0, 0.0, 1.0},
            rush::Vec3d{5.0, 2.0, 1.0}
    );

    rush::BezierCurve<2, 4, 3, double> curve(seg1, seg2);

    requireSimilar(curve.fetchDerivative(0.49999, true),
                   curve.fetchDerivative(0.50001, true));
}