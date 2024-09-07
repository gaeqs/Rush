//
// Created by gaeqs on 5/7/24.
//

#ifndef PERMANENT_HEAP_POOL_H
#define PERMANENT_HEAP_POOL_H

#include <bitset>
#include <cstdint>
#include <optional>
#include <rush/allocator/stack_allocator.h>

namespace rush {

    /**
     * Represents a simple memory pool where users can allocate but no deallocate data.
     *
     * This implementation allows the user to define the size of the
     * pool in the constructor.
     */
    class PermanentHeapPool {
        size_t _size;
        size_t _occupiedAmount;
        char* _data;

    public:
        /**
         * Creates a new pool.
         */
        PermanentHeapPool(size_t size)
                : _size(size), _occupiedAmount(0), _data(new char[size]) {
        }

        PermanentHeapPool(const PermanentHeapPool& other) = delete;

        ~PermanentHeapPool() {
            delete[] _data;
        }

        /**
         * @return the amount of allocated bytes.
         */
        [[nodiscard]] size_t occupied() const {
            return _occupiedAmount;
        }

        [[nodiscard]] void* pivot() const {
            return &_data[_occupiedAmount];
        }

        [[nodiscard]] bool canAllocate(size_t amount) const {
            return _occupiedAmount + amount < _size;
        }

        [[nodiscard]] bool isFull() const {
            return _occupiedAmount >= _size;
        }

        /**
         * Tries to allocate the given amount of bytes from this memory pool.
         * If no data is available, this method returns empty.
         * @return the allocated data or empty.
         */
        std::optional<void*> allocate(size_t amount) {
            if (_occupiedAmount + amount > _size) return {};
            auto ptr = _occupiedAmount;
            _occupiedAmount += amount;
            return &_data[ptr];
        }
    };
}
#endif //PERMANENT_HEAP_POOL_H
