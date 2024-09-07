//
// Created by gaeqs on 5/7/24.
//

#include "test_common.h"

TEST_CASE("Pool basics", "[pool]") {
    constexpr size_t AMOUNT = 20;
    rush::Pool<AMOUNT, sizeof(uint32_t)> pool;

    REQUIRE(pool.occupied() == 0);

    std::vector<uint32_t*> vector;

    for (size_t i = 0; i < AMOUNT; ++i) {
        std::optional<void*> optional = pool.allocate();
        REQUIRE(optional.has_value());
        auto* value = static_cast<uint32_t*>(optional.value());
        std::construct_at(value, 0);

        for (auto other: vector) {
            REQUIRE(other != value);
        }

        vector.push_back(value);
    }

    REQUIRE(pool.occupied() == AMOUNT);
    REQUIRE(!pool.allocate().has_value());
    REQUIRE(pool.occupied() == AMOUNT);

    for (auto ptr: vector) {
        std::destroy_at(ptr);
        REQUIRE(pool.deallocate(ptr));
    }

    REQUIRE(pool.occupied() == 0);
}
