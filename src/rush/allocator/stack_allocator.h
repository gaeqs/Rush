//
// Created by gaelr on 13/08/23.
//

#ifndef RUSH_STACK_ALLOCATOR_H
#define RUSH_STACK_ALLOCATOR_H

#include <array>
#include <cstddef>

namespace rush {

    struct StaticAllocator {

        template<size_t Size, typename Type>
        struct AllocatedData {

            using AllocType = Type;
            using Storage = std::array<Type, Size>;

            Storage data;

            static constexpr size_t size() {
                return Size;
            }

            inline Type& operator[](size_t i) {
                return data[i];
            }

            inline const Type& operator[](size_t i) const {
                return data[i];
            }

            inline Type* toPointer() {
                return data.data();
            }

            inline const Type* toPointer() const {
                return data.data();
            }

            inline Storage::iterator begin() {
                return data.begin();
            }

            inline Storage::iterator end() {
                return data.end();
            }

            inline Storage::const_iterator cbegin() const {
                return data.cbegin();
            };

            inline Storage::const_iterator cend() const {
                return data.cend();
            };

            inline Storage::reverse_iterator rbegin() {
                return data.rbegin();
            };

            inline Storage::reverse_iterator rend() {
                return data.rend();
            };

            inline Storage::const_reverse_iterator crbegin() const {
                return data.crbegin();
            };

            inline Storage::const_reverse_iterator crend() const {
                return data.crend();
            };

        };
    };

}

#endif //RUSH_STACK_ALLOCATOR_H
