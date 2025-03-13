//
// Created by gaelr on 13/08/23.
//

#include <catch2/catch_test_macros.hpp>
#include <catch2/benchmark/catch_benchmark.hpp>
#include <iostream>
#include <random>
#include <rush/rush.h>

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

TEST_CASE("Big matrix multiplication (double)", "[!benchmark][matrix]") {
    using BigMat = rush::Mat<1000, 1000, double, rush::MatDenseRep, rush::HeapAllocator>;
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

        meter.measure([a, b]() {
            return a * b;
        });
    };
}

TEST_CASE("Big sparse matrix multiplication (double)", "[!benchmark][matrix]") {
    using BigMat = rush::Mat<1000, 1000, double, rush::MatDenseRep, rush::HeapAllocator>;
    BENCHMARK_ADVANCED("1000 0.3%")(Catch::Benchmark::Chronometer meter) {
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_real_distribution<> distr(0.0, 1.0);

        BigMat a([&](size_t c, size_t r) {
            return c == r ? distr(gen) : (distr(gen) < 0.003 ? distr(gen) : 0.0);
        });
        BigMat b([&](size_t c, size_t r) {
            return c == r ? distr(gen) : (distr(gen) < 0.003 ? distr(gen) : 0.0);
        });

        meter.measure([a, b]() {
            return a * b;
        });
    };
    BENCHMARK_ADVANCED("1000 3%")(Catch::Benchmark::Chronometer meter) {
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_real_distribution<> distr(0.0, 1.0);

        BigMat a([&](size_t c, size_t r) {
            return c == r ? distr(gen) : (distr(gen) < 0.03 ? distr(gen) : 0.0);
        });
        BigMat b([&](size_t c, size_t r) {
            return c == r ? distr(gen) : (distr(gen) < 0.03 ? distr(gen) : 0.0);
        });

        meter.measure([a, b]() {
            return a * b;
        });
    };
    BENCHMARK_ADVANCED("1000 50%")(Catch::Benchmark::Chronometer meter) {
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_real_distribution<> distr(0.0, 1.0);

        BigMat a([&](size_t c, size_t r) {
            return c == r ? distr(gen) : (distr(gen) < 0.5 ? distr(gen) : 0.0);
        });
        BigMat b([&](size_t c, size_t r) {
            return c == r ? distr(gen) : (distr(gen) < 0.5 ? distr(gen) : 0.0);
        });

        meter.measure([a, b]() {
            return a * b;
        });
    };
}

TEST_CASE("Big dense LU decomposition (double)", "[!benchmark][matrix]") {
    BENCHMARK_ADVANCED("100x100 0.3%")(Catch::Benchmark::Chronometer meter) {
        using BigMat = rush::Mat<100, 100, double, rush::MatDenseRep, rush::HeapAllocator>;
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_real_distribution<> distr(0.0, 1.0);

        BigMat a([&](size_t c, size_t r) {
            return c == r ? distr(gen) : (distr(gen) < 0.003 ? distr(gen) : 0.0);
        });

        meter.measure([&a]() {
            return a.luDecomposed();
        });
    };
    BENCHMARK_ADVANCED("300x300 0.3%")(Catch::Benchmark::Chronometer meter) {
        using BigMat = rush::Mat<300, 300, double, rush::MatDenseRep, rush::HeapAllocator>;
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_real_distribution<> distr(0.0, 1.0);

        BigMat a([&](size_t c, size_t r) {
            return c == r ? distr(gen) : (distr(gen) < 0.003 ? distr(gen) : 0.0);
        });

        meter.measure([&a]() {
            return a.luDecomposed();
        });
    };
    BENCHMARK_ADVANCED("1000x1000 0.3%")(Catch::Benchmark::Chronometer meter) {
        using BigMat = rush::Mat<1000, 1000, double, rush::MatDenseRep, rush::HeapAllocator>;
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_real_distribution<> distr(0.0, 1.0);

        BigMat a([&](size_t c, size_t r) {
            return c == r ? distr(gen) : (distr(gen) < 0.003 ? distr(gen) : 0.0);
        });

        meter.measure([&a]() {
            return a.luDecomposed();
        });
    };
}

TEST_CASE("Big sparse LU decomposition (double)", "[!benchmark][matrix]") {
    BENCHMARK_ADVANCED("100x100 0.3%")(Catch::Benchmark::Chronometer meter) {
        using BigMat = rush::Mat<100, 100, double, rush::MatSparseRep, rush::HeapAllocator>;
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_real_distribution<> distr(0.0, 1.0);

        BigMat a([&](size_t c, size_t r) {
            return c == r ? distr(gen) : (distr(gen) < 0.003 ? distr(gen) : 0.0);
        });

        meter.measure([&a]() {
            return a.luDecomposed();
        });
    };
    BENCHMARK_ADVANCED("100x100 3%")(Catch::Benchmark::Chronometer meter) {
        using BigMat = rush::Mat<100, 100, double, rush::MatSparseRep, rush::HeapAllocator>;
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_real_distribution<> distr(0.0, 1.0);

        BigMat a([&](size_t c, size_t r) {
            return c == r ? distr(gen) : (distr(gen) < 0.03 ? distr(gen) : 0.0);
        });

        meter.measure([&a]() {
            return a.luDecomposed();
        });
    };
    BENCHMARK_ADVANCED("100x100 50%")(Catch::Benchmark::Chronometer meter) {
        using BigMat = rush::Mat<100, 100, double, rush::MatSparseRep, rush::HeapAllocator>;
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_real_distribution<> distr(0.0, 1.0);

        BigMat a([&](size_t c, size_t r) {
            return c == r ? distr(gen) : (distr(gen) < 0.5 ? distr(gen) : 0.0);
        });

        meter.measure([&a]() {
            return a.luDecomposed();
        });
    };
    BENCHMARK_ADVANCED("300x300 0.3%")(Catch::Benchmark::Chronometer meter) {
        using BigMat = rush::Mat<300, 300, double, rush::MatSparseRep, rush::HeapAllocator>;
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_real_distribution<> distr(0.0, 1.0);

        BigMat a([&](size_t c, size_t r) {
            return c == r ? distr(gen) : (distr(gen) < 0.003 ? distr(gen) : 0.0);
        });

        meter.measure([&a]() {
            return a.luDecomposed();
        });
    };
    BENCHMARK_ADVANCED("1000x100 0.3%")(Catch::Benchmark::Chronometer meter) {
        using BigMat = rush::Mat<1000, 1000, double, rush::MatSparseRep, rush::HeapAllocator>;
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_real_distribution<> distr(0.0, 1.0);

        BigMat a([&](size_t c, size_t r) {
            return c == r ? distr(gen) : (distr(gen) < 0.003 ? distr(gen) : 0.0);
        });

        meter.measure([&a]() {
            return a.luDecomposed();
        });
    };
}

TEST_CASE("Big sparse LU decomposition (float)", "[!benchmark][matrix]") {
    BENCHMARK_ADVANCED("100x100 0.3%")(Catch::Benchmark::Chronometer meter) {
        using BigMat = rush::Mat<100, 100, float, rush::MatSparseRep, rush::HeapAllocator>;
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_real_distribution<> distr(0.0, 1.0);

        BigMat a([&](size_t c, size_t r) {
            return c == r ? distr(gen) : (distr(gen) < 0.003 ? distr(gen) : 0.0);
        });

        meter.measure([&a]() {
            return a.luDecomposed();
        });
    };
    BENCHMARK_ADVANCED("100x100 3%")(Catch::Benchmark::Chronometer meter) {
        using BigMat = rush::Mat<100, 100, float, rush::MatSparseRep, rush::HeapAllocator>;
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_real_distribution<> distr(0.0, 1.0);

        BigMat a([&](size_t c, size_t r) {
            return c == r ? distr(gen) : (distr(gen) < 0.03 ? distr(gen) : 0.0);
        });

        meter.measure([&a]() {
            return a.luDecomposed();
        });
    };
    BENCHMARK_ADVANCED("100x100 50%")(Catch::Benchmark::Chronometer meter) {
        using BigMat = rush::Mat<100, 100, float, rush::MatSparseRep, rush::HeapAllocator>;
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_real_distribution<> distr(0.0, 1.0);

        BigMat a([&](size_t c, size_t r) {
            return c == r ? distr(gen) : (distr(gen) < 0.5 ? distr(gen) : 0.0);
        });

        meter.measure([&a]() {
            return a.luDecomposed();
        });
    };
    BENCHMARK_ADVANCED("300x300 0.3%")(Catch::Benchmark::Chronometer meter) {
        using BigMat = rush::Mat<300, 300, float, rush::MatSparseRep, rush::HeapAllocator>;
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_real_distribution<> distr(0.0, 1.0);

        BigMat a([&](size_t c, size_t r) {
            return c == r ? distr(gen) : (distr(gen) < 0.003 ? distr(gen) : 0.0);
        });

        meter.measure([&a]() {
            return a.luDecomposed();
        });
    };
    BENCHMARK_ADVANCED("1000x1000 0.3%")(Catch::Benchmark::Chronometer meter) {
        using BigMat = rush::Mat<1000, 1000, float, rush::MatSparseRep, rush::HeapAllocator>;
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_real_distribution<> distr(0.0, 1.0);

        BigMat a([&](size_t c, size_t r) {
            return c == r ? distr(gen) : (distr(gen) < 0.003 ? distr(gen) : 0.0);
        });

        meter.measure([&a]() {
            return a.luDecomposed();
        });
    };
}
