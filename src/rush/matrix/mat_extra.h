//
// Created by gaelr on 31/07/2023.
//

#ifndef RUSH_MAT_EXTRA_H
#define RUSH_MAT_EXTRA_H

#include <rush/vector/vec_base.h>
#include <rush/vector/vec_ref.h>
#include <ostream>

template<size_t C, size_t R, typename Type>
std::ostream& operator<<(std::ostream& os, const rush::Mat<C, R, Type>& m) {
    for (int r = 0; r < R; ++r) {
        os << "|";
        for (int c = 0; c < C; ++c) {
            os << m(c, r);
            if (c < C - 1) {
                os << ",\t";
            }
        }
        os << "|";
        if (r < R - 1) {
            os << "\n";
        }
    }
    return os;
}

template<size_t C, size_t R, typename Type>
struct std::hash<rush::Mat<C, R, Type>> {
    std::size_t operator()(rush::Mat<C, R, Type> const& s) const noexcept {
        std::hash<Type> hasher;
        size_t hash = 0;
        for (int c = 0; c < C; ++c) {
            for (int r = 0; r < R; ++r) {
                hash ^= hasher(s(c, r)) + 0x9e3779b9
                        + (hash << 6) + (hash >> 2);
            }
        }
        return hash;
    }
};

#endif //RUSH_MAT_EXTRA_H
