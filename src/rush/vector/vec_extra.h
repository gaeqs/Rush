//
// Created by gaelr on 31/07/2023.
//

#ifndef RUSH_VEC_EXTRA_H
#define RUSH_VEC_EXTRA_H

#include <rush/vector/vec_base.h>
#include <rush/vector/vec_ref.h>
#include <ostream>

template<size_t Size, typename Type>
std::ostream& operator<<(std::ostream& os, const rush::Vec<Size, Type>& v) {
    os << "(";
    for (size_t i = 0; i < Size; ++i) {
        os << v[i];
        if (i < Size - 1) {
            os << ", ";
        }
    }
    os << ")";
    return os;
}

template<size_t Size, typename Type>
std::ostream& operator<<(std::ostream& os, const rush::VecRef<Size, Type>& r) {
    os << "(";
    for (size_t i = 0; i < Size; ++i) {
        os << *r.references[i];
        if (i < Size - 1) {
            os << ", ";
        }
    }
    os << ")";
    return os;
}


template<size_t Size, typename Type>
struct std::hash<rush::Vec<Size, Type>> {
    std::size_t operator()(rush::Vec<Size, Type> const& s) const noexcept {
        std::hash<Type> hasher;
        size_t hash = 0;
        for (int i = 0; i < Size; ++i) {
            hash ^= hasher(s[i]) + 0x9e3779b9 + (hash << 6) + (hash >> 2);
        }
        return hash;
    }
};

// REGION EXTRA OPERATIONS

template<size_t Size, typename Type>
requires rush::HasAdd<Type>
inline rush::Vec<Size, Type> operator+(
        const Type& s,
        const rush::Vec<Size, Type>& v) {
    rush::Vec<Size, Type> result;
    for (int i = 0; i < Size; ++i) {
        result[i] = s + v[i];
    }
    return result;
}

template<size_t Size, typename Type>
requires rush::HasSub<Type>
inline rush::Vec<Size, Type> operator-(
        const Type& s,
        const rush::Vec<Size, Type>& v) {
    rush::Vec<Size, Type> result;
    for (int i = 0; i < Size; ++i) {
        result[i] = s - v[i];
    }
    return result;
}

template<size_t Size, typename Type>
requires rush::HasMul<Type>
inline rush::Vec<Size, Type> operator*(
        const Type& s,
        const rush::Vec<Size, Type>& v) {
    rush::Vec<Size, Type> result;
    for (int i = 0; i < Size; ++i) {
        result[i] = s * v[i];
    }
    return result;
}

template<size_t Size, typename Type>
requires rush::HasDiv<Type>
inline rush::Vec<Size, Type> operator/(
        const Type& s,
        const rush::Vec<Size, Type>& v) {
    rush::Vec<Size, Type> result;
    for (int i = 0; i < Size; ++i) {
        result[i] = s / v[i];
    }
    return result;
}

// ENDREGION

#endif //RUSH_VEC_EXTRA_H
