//
// Created by gaeqs on 5/7/24.
//

#ifndef POOL_H
#define POOL_H

#include <bitset>
#include <cstdint>
#include <optional>
#include <rush/allocator/stack_allocator.h>

namespace rush {
    /**
     * Represents a simple memory pool where users can allocate and deallocate data.
     *
     * This memory pool is split in chunks.
     * Users can allocate one chunk per each allocation call.
     *
     * @tparam Size the amount of chunks inside the pool.
     * @tparam ChunkSize the size of the chunk.
     * @tparam Allocator the allocator for the raw memory.
     */
    template<size_t Size, size_t ChunkSize, typename Allocator = StaticAllocator>
    class Pool {
        typename Allocator::template AllocatedData<Size * ChunkSize, char> _data;
        std::bitset<Size> _occupied;
        size_t _occupiedAmount;

    public:
        /**
         * Creates a new pool.
         */
        Pool() : _data(), _occupied(), _occupiedAmount(0) {
        }

        Pool(const Pool& other) = delete;

        /**
         * @return the amount of allocated chunks.
         */
        [[nodiscard]] size_t occupied() const {
            return _occupiedAmount;
        }

        /**
         * Tries to allocate a chunk from this memory pool.
         * If no chunks are available, this method returns empty.
         * @return the allocated chunk or empty.
         */
        [[nodiscard]] std::optional<void*> allocate() {
            if (_occupiedAmount >= Size) return {};
            size_t index = 0;
            while (_occupied[index]) ++index;
            _occupied.flip(index);
            ++_occupiedAmount;
            return &_data[index * ChunkSize];
        }

        /**
         * Deallocates the given memory chunk.
         * @param ptr the pointer.
         * @return whether the chunk has been deallocated.
         */
        bool deallocate(const void* ptr) {
            const char* ptrC = static_cast<const char*>(ptr);
            char* start = _data.toPointer();
            if (start > ptr) return false;
            uint64_t index = (ptrC - start) / ChunkSize;
            if (index >= Size) return false;
            if (!_occupied[index]) return false;
            _occupied.flip(index);
            --_occupiedAmount;
            return true;
        }
    };
}
#endif //POOL_H
