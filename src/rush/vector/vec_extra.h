//
// Created by gaelr on 31/07/2023.
//

#ifndef RUSH_VEC_EXTRA_H
#define RUSH_VEC_EXTRA_H

#include <rush/vector/vec_base.h>
#include <rush/vector/vec_ref.h>
#include <ostream>

template<size_t Size, typename Type, typename Allocator>
std::ostream&
operator<<(std::ostream& os, const rush::Vec<Size, Type, Allocator>& v) {
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

template<size_t Size, typename Type, typename Allocator>
struct std::hash<rush::Vec<Size, Type, Allocator>> {
    std::size_t
    operator()(rush::Vec<Size, Type, Allocator> const& s) const noexcept {
        std::hash<Type> hasher;
        size_t h = 0;
        for (size_t i = 0; i < Size; ++i) {
            h ^= hasher(s[i]) + 0x9e3779b9 + (h << 6) + (h >> 2);
        }
        return h;
    }
};

// REGION EXTRA OPERATIONS

template<size_t Size, typename Type, typename Allocator>
    requires rush::HasAdd<Type>
rush::Vec<Size, Type, Allocator> operator+(
    const Type& s,
    const rush::Vec<Size, Type, Allocator>& v) {
    rush::Vec<Size, Type, Allocator> result;
    for (size_t i = 0; i < Size; ++i) {
        result[i] = s + v[i];
    }
    return result;
}

template<size_t Size, typename Type, typename Allocator>
    requires rush::HasSub<Type>
rush::Vec<Size, Type, Allocator> operator-(
    const Type& s,
    const rush::Vec<Size, Type, Allocator>& v) {
    rush::Vec<Size, Type, Allocator> result;
    for (size_t i = 0; i < Size; ++i) {
        result[i] = s - v[i];
    }
    return result;
}

template<size_t Size, typename Type, typename Allocator>
    requires rush::HasMul<Type>
rush::Vec<Size, Type, Allocator> operator*(
    const Type& s,
    const rush::Vec<Size, Type, Allocator>& v) {
    rush::Vec<Size, Type, Allocator> result;
    for (size_t i = 0; i < Size; ++i) {
        result[i] = s * v[i];
    }
    return result;
}

template<size_t Size, typename Type, typename Allocator>
    requires rush::HasDiv<Type>
rush::Vec<Size, Type, Allocator> operator/(
    const Type& s,
    const rush::Vec<Size, Type, Allocator>& v) {
    rush::Vec<Size, Type, Allocator> result;
    for (size_t i = 0; i < Size; ++i) {
        result[i] = s / v[i];
    }
    return result;
}

template<size_t Size, typename Type, typename Allocator>
    requires rush::HasShl<Type>
rush::Vec<Size, Type, Allocator> operator<<(
    const Type& s,
    const rush::Vec<Size, Type, Allocator>& v) {
    rush::Vec<Size, Type, Allocator> result;
    for (size_t i = 0; i < Size; ++i) {
        result[i] = s << v[i];
    }
    return result;
}

template<size_t Size, typename Type, typename Allocator>
    requires rush::HasShr<Type>
rush::Vec<Size, Type, Allocator> operator>>(
    const Type& s,
    const rush::Vec<Size, Type, Allocator>& v) {
    rush::Vec<Size, Type, Allocator> result;
    for (size_t i = 0; i < Size; ++i) {
        result[i] = s >> v[i];
    }
    return result;
}

template<size_t Size, typename Type, typename Allocator>
    requires rush::HasBitAnd<Type>
rush::Vec<Size, Type, Allocator> operator&(
    const Type& s,
    const rush::Vec<Size, Type, Allocator>& v) {
    rush::Vec<Size, Type, Allocator> result;
    for (size_t i = 0; i < Size; ++i) {
        result[i] = s & v[i];
    }
    return result;
}

template<size_t Size, typename Type, typename Allocator>
    requires rush::HasBitOr<Type>
rush::Vec<Size, Type, Allocator> operator|(
    const Type& s,
    const rush::Vec<Size, Type, Allocator>& v) {
    rush::Vec<Size, Type, Allocator> result;
    for (size_t i = 0; i < Size; ++i) {
        result[i] = s | v[i];
    }
    return result;
}

template<size_t Size, typename Type, typename Allocator>
    requires rush::HasBitXor<Type>
rush::Vec<Size, Type, Allocator> operator^(
    const Type& s,
    const rush::Vec<Size, Type, Allocator>& v) {
    rush::Vec<Size, Type, Allocator> result;
    for (size_t i = 0; i < Size; ++i) {
        result[i] = s ^ v[i];
    }
    return result;
}

template<size_t Size, typename Type, typename Allocator>
    requires rush::HasAnd<Type>
rush::Vec<Size, Type, Allocator> operator&&(
    const Type& s,
    const rush::Vec<Size, Type, Allocator>& v) {
    rush::Vec<Size, Type, Allocator> result;
    for (size_t i = 0; i < Size; ++i) {
        result[i] = s && v[i];
    }
    return result;
}

template<size_t Size, typename Type, typename Allocator>
    requires rush::HasOr<Type>
rush::Vec<Size, Type, Allocator> operator||(
    const Type& s,
    const rush::Vec<Size, Type, Allocator>& v) {
    rush::Vec<Size, Type, Allocator> result;
    for (size_t i = 0; i < Size; ++i) {
        result[i] = s || v[i];
    }
    return result;
}

// ENDREGION

#endif //RUSH_VEC_EXTRA_H
