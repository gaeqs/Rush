#include <iostream>
#include <unordered_set>

#include <catch2/catch_test_macros.hpp>
#include <rush/vector/vec.h>

using V1 = rush::Vec<1, int>;
using V2 = rush::Vec<2, int>;
using V3 = rush::Vec<3, int>;
using V5 = rush::Vec<5, int>;

TEST_CASE("Vector creation", "[vector]") {
    REQUIRE_NOTHROW(V5());
    REQUIRE_NOTHROW(V5(1, 2, 3, 4, 5));
    REQUIRE_NOTHROW(V2(V5(1, 2, 3, 4, 5)));
    REQUIRE(V2(V5(1, 2, 3, 4, 5)) == V2(1, 2));

    V2 v;
    REQUIRE(v[0] == 0);
    REQUIRE(v[1] == 0);
    REQUIRE(V5([](size_t i) { return static_cast<int>(i); }) ==
            V5(0, 1, 2, 3, 4));
}

TEST_CASE("Vector access", "[vector]") {
    V5 o = {4, 3, 2, 1, 0};
    REQUIRE(o.size() == 5);
    REQUIRE((*o(1, 1)).size() == 2);
    REQUIRE(o == o(0, 1, 2, 3, 4));
    REQUIRE(V2(3, 2) == o(1, 2));

    o(3, 2) = V2(20, 30);
    REQUIRE(o == V5(4, 3, 30, 20, 0));

    o[3] = 50;
    REQUIRE(o == V5(4, 3, 30, 50, 0));
    REQUIRE(o[4] == 0);

    o(0, 1) = o(1, 2);
    REQUIRE(o == V5(3, 30, 30, 50, 0));

    // Check aliasing
    V2 swap = {0, 1};
    swap(0, 1) = swap(1, 0);
    REQUIRE(swap == V2(1, 0));

    REQUIRE_NOTHROW(o[0]);
    REQUIRE_NOTHROW(o[o.size() - 1]);
}

TEST_CASE("Vector length", "[vector]") {
    V5 o = {4, 3, 2, 1, 0};
    float length2 = 1.0f + 4.0f + 9.0f + 16.0f;

    REQUIRE(o.squaredLength() == length2);
    REQUIRE(o.length() != std::sqrt(length2));
    REQUIRE(o.length<float>() == std::sqrt(length2));
}

TEST_CASE("Vector operations", "[vector]") {
    V5 o = {4, 3, 2, 1, 0};
    V5 p = {10, 12, 14, 0, 0};

    SECTION("Addition") {
        REQUIRE(o + p == V5(14, 15, 16, 1, 0));
        REQUIRE(o(2, 3) + p(3, 2) == V2(2, 15));
        REQUIRE(o(4) + p(4) == V1());
    }

    SECTION("Subtraction") {
        REQUIRE(o - p == V5(-6, -9, -12, 1, 0));
        REQUIRE(o(2, 3) - p(3, 2) == V2(2, -13));
        REQUIRE(o(4) - p(4) == V1());
    }

    SECTION("Multiplication") {
        REQUIRE(o * p == V5(40, 36, 28, 0, 0));
        REQUIRE(o(2, 3) * p(3, 2) == V2(0, 14));
        REQUIRE(o(4) * p(4) == V1());
    }

    o = {10, 12, 14, 0, 0};
    p = {4, 3, 2, 1, 2};

    SECTION("Division") {
        REQUIRE(o / p == V5(2, 4, 7, 0, 0));
        REQUIRE(o(2, 3) / p(3, 2) == V2(14, 0));
        REQUIRE(o(4) / p(4) == rush::Vec<1, int>());
    }

    SECTION("Dot") {
        REQUIRE(o % p == o.dot(p));
        REQUIRE(o % p == 40 + 36 + 28);
    }

    SECTION("Cross") {
        V3 a = {10, 20, 30};
        V3 b = {0, 0, 1};
        REQUIRE((a ^ b) == a.cross(b));
        REQUIRE((a ^ b) == V3(20, -10, 0));
    }

    SECTION("Scalar") {
        REQUIRE(o + 2 == V5(12, 14, 16, 2, 2));
        REQUIRE(o - 2 == V5(8, 10, 12, -2, -2));
        REQUIRE(o * 2 == V5(20, 24, 28, 0, 0));
        REQUIRE(o / 2 == V5(5, 6, 7, 0, 0));

        REQUIRE(o + 2 == 2 + o);
        REQUIRE(o - 2 == -2 + o);
        REQUIRE(o * 2 == 2 * o);
    }
}

TEST_CASE("Vector assign operations", "[vector]") {
    SECTION("Addition") {
        V5 o = {10, 12, 14, 0, 0};
        V5 p = {4, 3, 2, 1, 2};
        V5 r = o;
        r += p;
        REQUIRE(r == o + p);
    }

    SECTION("Subtraction") {
        V5 o = {10, 12, 14, 0, 0};
        V5 p = {4, 3, 2, 1, 2};
        V5 r = o;
        r -= p;
        REQUIRE(r == o - p);
    }

    SECTION("Multiplication") {
        V5 o = {10, 12, 14, 0, 0};
        V5 p = {4, 3, 2, 1, 2};
        V5 r = o;
        r *= p;
        REQUIRE(r == o * p);
    }

    SECTION("Division") {
        V5 o = {10, 12, 14, 0, 0};
        V5 p = {4, 3, 2, 1, 2};
        V5 r = o;
        r /= p;
        REQUIRE(r == o / p);
    }
}

TEST_CASE("Vector iterator", "[vector]") {
    V5 o = {4, 3, 2, 1, 0};

    o.y();

    size_t i = 0;
    for (const auto& item: o) {
        REQUIRE(item == o[i++]);
    }
}

TEST_CASE("Vector print", "[vector]") {
    V5 o = {4, 3, 2, 1, 0};

    std::stringstream ss;
    ss << o;

    REQUIRE(ss.str() == "(4, 3, 2, 1, 0)");
}