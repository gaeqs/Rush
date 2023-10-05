//
// Created by gaelr on 25/09/23.
//

#include "test_common.h"

TEST_CASE("Factorial", "[scalar]") {
    REQUIRE(rush::factorial(-1) == 0);
    REQUIRE(rush::factorial(0) == 0);

    uint64_t n = 1;

    for (uint64_t i = 1; i < 20; i++) {
        n *= i;
        REQUIRE(rush::factorial(i) == n);
    }
}

TEST_CASE("Binomial", "[scalar]") {
    REQUIRE(rush::binomial(10, 5) == 252);
    REQUIRE(rush::binomial(8, 3) == 56);
    REQUIRE(rush::binomial<uint64_t>(100, 5) == 75287520);
    REQUIRE(rush::binomial<uint64_t>(100, 100) == 1);
    REQUIRE(rush::binomial<uint64_t>(100, 500) == 0);
    REQUIRE(rush::binomial<int64_t>(-1, -2) == 1);
}