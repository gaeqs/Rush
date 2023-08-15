//
// Created by gaelr on 13/08/23.
//

#include <catch2/catch_test_macros.hpp>
#include <catch2/benchmark/catch_benchmark.hpp>
#include <iostream>
#include <random>
#include <rush/rush.h>

constexpr size_t ROW_DIM = 1000;

using V1 = rush::Vec<1, int>;
using V2 = rush::Vec<2, int>;
using V3 = rush::Vec<3, int>;
using V5 = rush::Vec<5, int>;
using Mat2 = rush::Mat<2, 2, int>;
using Mat3 = rush::Mat<3, 3, int>;
using Mat4 = rush::Mat<4, 4, int>;
using Mat2f = rush::Mat<2, 2, float>;
using Mat3f = rush::Mat<3, 3, float>;
using Mat4f = rush::Mat<4, 4, float>;

TEST_CASE("Big matrix multiplication (double)", "[!benchmark]") {
    using BigMat = rush::Mat<ROW_DIM, ROW_DIM, double, rush::HeapAllocator>;
    BENCHMARK_ADVANCED("1000")(Catch::Benchmark::Chronometer meter) {
            std::random_device rd;
            std::mt19937 gen(rd());
            std::uniform_real_distribution<> distr(0.0, 1.0);

            BigMat a([&](size_t c, size_t r) {
                return distr(gen);
            });
            BigMat b([&](size_t c, size_t r) {
                return distr(gen);
            });

            meter.measure([a, b]() { return a * b; });
        };
}
