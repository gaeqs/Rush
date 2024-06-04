//
// Created by gaeqs on 5/7/24.
//

#ifndef PERMANENT_POOL_H
#define PERMANENT_POOL_H

#include <bitset>
#include <cstdint>
#include <optional>
#include <rush/allocator/stack_allocator.h>

namespace rush {

    /**
     * Represents a simple memory pool where users can allocate but no deallocate data.
     *
     * @tparam Size the amount of chunks inside the pool.
     * @tparam Allocator the allocator for the raw memory.
     */
    template<size_t Size, typename Allocator = StaticAllocator>
    class PermanentPool {
        typename Allocator::template AllocatedData<Size, char> _data;
        size_t _occupiedAmount;

    public:
        /**
         * Creates a new pool.
         */
        PermanentPool() : _data(), _occupiedAmount(0) {
        }

        PermanentPool(const PermanentPool& other) = delete;

        /**
         * @return the amount of allocated bytes.
         */
        [[nodiscard]] size_t occupied() const {
            return _occupiedAmount;
        }

        /**
         * Tries to allocate the given amount of bytes from this memory pool.
         * If no data is available, this method returns empty.
         * @return the allocated data or empty.
         */
        [[nodiscard]] std::optional<void*> allocate(size_t amount) {
            if (_occupiedAmount + amount >= Size) return {};
            auto ptr = _occupiedAmount;
            _occupiedAmount += amount;
            return &_data[ptr];
        }
    };
}
#endif //PERMANENT_POOL_H
