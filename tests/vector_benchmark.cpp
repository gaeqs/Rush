//
// Created by gaelr on 13/08/23.
//

#include <catch2/catch_test_macros.hpp>
#include <catch2/benchmark/catch_benchmark.hpp>
#include <rush/rush.h>

using V1f = rush::Vec<1, float>;
using V2f = rush::Vec<2, float>;
using V3f = rush::Vec<3, float>;
using V5f = rush::Vec<5, float>;
using V1d = rush::Vec<1, double>;
using V2d = rush::Vec<2, double>;
using V3d = rush::Vec<3, double>;
using V4d = rush::Vec<4, double>;
using V5d = rush::Vec<5, double>;

TEST_CASE("Vector inverse length (float)", "[!benchmark][vector]") {
    V5f o = {4.0f, 3.0f, 2.0f, 1.0f, 0.0f};
    BENCHMARK("Manual") { return 1.0f / o.length(); };
    BENCHMARK("High - Intrinsics")
                { return o.inverseLength<float, rush::HIGH_INTRINSICS>(); };
    BENCHMARK("High - General")
                { return o.inverseLength<float, rush::HIGH_GENERAL>(); };
    BENCHMARK("Low - Intrinsics")
                { return o.inverseLength<float, rush::LOW_INTRINSICS>(); };
    BENCHMARK("Low - General")
                { return o.inverseLength<float, rush::LOW_GENERAL>(); };
}

TEST_CASE("Vector inverse length (double)", "[!benchmark][vector]") {
    V5d o = {4.0, 3.0, 2.0, 1.0, 0.0};
    BENCHMARK("Manual") { return 1.0 / o.length(); };
    BENCHMARK("High - Intrinsics")
                { return o.inverseLength<double, rush::HIGH_INTRINSICS>(); };
    BENCHMARK("High - General")
                { return o.inverseLength<double, rush::HIGH_GENERAL>(); };
    BENCHMARK("Low - Intrinsics")
                { return o.inverseLength<double, rush::LOW_INTRINSICS>(); };
    BENCHMARK("Low - General")
                { return o.inverseLength<double, rush::LOW_GENERAL>(); };
}

TEST_CASE("Vector normalize (double)", "[!benchmark][vector]") {
    V5d o = {4.0, 3.0, 2.0, 1.0, 0.0};
    BENCHMARK("Manual") { return o / o.length(); };
    BENCHMARK("High - Intrinsics")
                { return o.normalized<double, rush::HIGH_INTRINSICS>(); };
    BENCHMARK("High - General")
                { return o.normalized<double, rush::HIGH_GENERAL>(); };
    BENCHMARK("Low - Intrinsics")
                { return o.normalized<double, rush::LOW_INTRINSICS>(); };
    BENCHMARK("Low - General")
                { return o.normalized<double, rush::LOW_GENERAL>(); };
}

TEST_CASE("Vector angle (3D)", "[!benchmark][vector]") {
    V3d o = {4.0, 3.0, 2.0};
    V3d p = {2.0, 10.0, 1.0};
    BENCHMARK("High - Intrinsics")
                { return o.angle<double, rush::HIGH_INTRINSICS>(p); };
    BENCHMARK("High - General")
                { return o.angle<double, rush::HIGH_GENERAL>(p); };
    BENCHMARK("Low - Intrinsics")
                { return o.angle<double, rush::LOW_INTRINSICS>(p); };
    BENCHMARK("Low - General")
                { return o.angle<double, rush::LOW_GENERAL>(p); };
}

TEST_CASE("Vector angle (4D)", "[!benchmark][vector]") {
    V4d o = {4.0, 3.0, 2.0, 5.0};
    V4d p = {2.0, 10.0, 1.0, -2.0};
    BENCHMARK("High - Intrinsics")
                { return o.angle<double, rush::HIGH_INTRINSICS>(p); };
    BENCHMARK("High - General")
                { return o.angle<double, rush::HIGH_GENERAL>(p); };
    BENCHMARK("Low - Intrinsics")
                { return o.angle<double, rush::LOW_INTRINSICS>(p); };
    BENCHMARK("Low - General")
                { return o.angle<double, rush::LOW_GENERAL>(p); };
}

