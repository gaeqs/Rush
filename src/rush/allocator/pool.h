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
    template<size_t Size, size_t TypeSize, typename Allocator = StaticAllocator>
    class Pool {
        typename Allocator::template AllocatedData<Size * TypeSize, char> _data;
        std::bitset<Size> _occupied;
        size_t _occupiedAmount;

    public:
        Pool() : _data(), _occupied(), _occupiedAmount(0) {
        }

        Pool(const Pool& other) = delete;

        [[nodiscard]] size_t occupied() const {
            return _occupiedAmount;
        }

        [[nodiscard]] std::optional<void*> allocate() {
            if (_occupiedAmount >= Size) return {};
            size_t index = 0;
            while (_occupied[index]) ++index;
            _occupied.flip(index);
            ++_occupiedAmount;
            return &_data[index * TypeSize];
        }

        bool deallocate(const void* ptr) {
            const char* ptrC = static_cast<const char*>(ptr);
            char* start = _data.toPointer();
            if (start > ptr) return false;
            uint64_t index = (ptrC - start) / TypeSize;
            if (index >= Size) return false;
            if (!_occupied[index]) return false;
            _occupied.flip(index);
            --_occupiedAmount;
            return true;
        }
    };
}
#endif //POOL_H
