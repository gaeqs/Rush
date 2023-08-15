//
// Created by gaelr on 13/08/23.
//

#ifndef RUSH_HEAP_ALLOCATOR_H
#define RUSH_HEAP_ALLOCATOR_H

#include <array>
#include <cstddef>
#include <memory>

namespace rush {

    struct HeapAllocator {

        template<size_t Size, typename Type>
        struct AllocatedData {

            using AllocType = Type;
            using Storage = std::array<Type, Size>;

            std::unique_ptr<Storage> data;

            AllocatedData() : data(std::make_unique<Storage>()) {
            }

            AllocatedData(const AllocatedData& other) :
                    data(std::make_unique<Storage>()) {
                std::copy(other.cbegin(), other.cend(), data->begin());
            }

            AllocatedData(AllocatedData&& other) noexcept:
                    data(std::move(other.data)) {
            }

            AllocatedData& operator=(const AllocatedData& other) {
                std::copy(other.cbegin(), other.cend(), data->begin());
                return *this;
            }

            AllocatedData& operator=(AllocatedData&& other) noexcept {
                data = std::move(other.data);
                return *this;
            }

            static constexpr size_t size() {
                return Size;
            }

            inline Type& operator[](size_t i) {
                return data->operator[](i);
            }

            inline const Type& operator[](size_t i) const {
                return data->operator[](i);
            }

            inline Type* toPointer() {
                return data->data();
            }

            inline const Type* toPointer() const {
                return data->data();
            }

            inline Storage::iterator begin() {
                return data->begin();
            }

            inline Storage::iterator end() {
                return data->end();
            }

            inline Storage::const_iterator cbegin() const {
                return data->cbegin();
            };

            inline Storage::const_iterator cend() const {
                return data->cend();
            };

            inline Storage::reverse_iterator rbegin() {
                return data->rbegin();
            };

            inline Storage::reverse_iterator rend() {
                return data->rend();
            };

            inline Storage::const_reverse_iterator crbegin() const {
                return data->crbegin();
            };

            inline Storage::const_reverse_iterator crend() const {
                return data->crend();
            };

        };
    };
}

#endif //RUSH_HEAP_ALLOCATOR_H
