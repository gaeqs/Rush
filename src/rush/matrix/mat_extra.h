//
// Created by gaelr on 31/07/2023.
//

#ifndef RUSH_MAT_EXTRA_H
#define RUSH_MAT_EXTRA_H

#include <ostream>

template<size_t C, size_t R, typename Type, typename Representation, typename Allocator>
std::ostream& operator<<(std::ostream& os, const rush::Mat<C, R, Type, Representation, Allocator>& m) {
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

template<size_t C, size_t R, typename Type, typename Representation, typename Allocator>
struct std::hash<rush::Mat<C, R, Type, Representation, Allocator>> {
    std::size_t operator()(rush::Mat<C, R, Type, Representation, Allocator> const& s) const noexcept {
        std::hash<Type> hasher;
        size_t h = 0;
        for (int c = 0; c < C; ++c) {
            for (int r = 0; r < R; ++r) {
                h ^= hasher(s(c, r)) + 0x9e3779b9
                        + (h << 6) + (h >> 2);
            }
        }
        return h;
    }
};

// REGION EXTRA OPERATIONS

template<size_t Columns, size_t Rows, typename Type, typename Representation, typename Allocator>
    requires rush::HasAdd<Type>
inline rush::Mat<Columns, Rows, Type, Representation, Allocator> operator+(
    const Type& s,
    const rush::Mat<Columns, Rows, Type, Representation, Allocator>& m) {
    rush::Mat<Columns, Rows, Type, Representation, Allocator> result;
    for (size_t i = 0; i < Columns; ++i) {
        result[i] = s + m[i];
    }
    return result;
}

template<size_t Columns, size_t Rows, typename Type, typename Representation, typename Allocator>
    requires rush::HasSub<Type>
inline rush::Mat<Columns, Rows, Type, Representation, Allocator> operator-(
    const Type& s,
    const rush::Mat<Columns, Rows, Type, Representation, Allocator>& m) {
    rush::Mat<Columns, Rows, Type, Representation, Allocator> result;
    for (size_t i = 0; i < Columns; ++i) {
        result[i] = s - m[i];
    }
    return result;
}

template<size_t Columns, size_t Rows, typename Type, typename Representation, typename Allocator>
    requires rush::HasMul<Type>
inline rush::Mat<Columns, Rows, Type, Representation, Allocator> operator*(
    const Type& s,
    const rush::Mat<Columns, Rows, Type, Representation, Allocator>& m) {
    rush::Mat<Columns, Rows, Type, Representation, Allocator> result;
    for (size_t i = 0; i < Columns; ++i) {
        result[i] = s * m[i];
    }
    return result;
}

template<size_t Columns, size_t Rows, typename Type, typename Representation, typename Allocator>
    requires rush::HasDiv<Type>
inline rush::Mat<Columns, Rows, Type, Representation, Allocator> operator/(
    const Type& s,
    const rush::Mat<Columns, Rows, Type, Representation, Allocator>& m) {
    rush::Mat<Columns, Rows, Type, Representation, Allocator> result;
    for (size_t i = 0; i < Columns; ++i) {
        result[i] = s / m[i];
    }
    return result;
}

template<size_t Columns, size_t Rows, typename Type, typename Representation, typename Allocator>
    requires rush::HasShl<Type>
inline rush::Mat<Columns, Rows, Type, Representation, Allocator> operator<<(
    const Type& s,
    const rush::Mat<Columns, Rows, Type, Representation, Allocator>& m) {
    rush::Mat<Columns, Rows, Type, Representation, Allocator> result;
    for (size_t i = 0; i < Columns; ++i) {
        result[i] = s << m[i];
    }
    return result;
}

template<size_t Columns, size_t Rows, typename Type, typename Representation, typename Allocator>
    requires rush::HasShr<Type>
inline rush::Mat<Columns, Rows, Type, Representation, Allocator> operator>>(
    const Type& s,
    const rush::Mat<Columns, Rows, Type, Representation, Allocator>& m) {
    rush::Mat<Columns, Rows, Type, Representation, Allocator> result;
    for (size_t i = 0; i < Columns; ++i) {
        result[i] = s >> m[i];
    }
    return result;
}

template<size_t Columns, size_t Rows, typename Type, typename Representation, typename Allocator>
    requires rush::HasBitAnd<Type>
inline rush::Mat<Columns, Rows, Type, Representation, Allocator> operator&(
    const Type& s,
    const rush::Mat<Columns, Rows, Type, Representation, Allocator>& m) {
    rush::Mat<Columns, Rows, Type, Representation, Allocator> result;
    for (size_t i = 0; i < Columns; ++i) {
        result[i] = s & m[i];
    }
    return result;
}

template<size_t Columns, size_t Rows, typename Type, typename Representation, typename Allocator>
    requires rush::HasBitOr<Type>
inline rush::Mat<Columns, Rows, Type, Representation, Allocator> operator|(
    const Type& s,
    const rush::Mat<Columns, Rows, Type, Representation, Allocator>& m) {
    rush::Mat<Columns, Rows, Type, Representation, Allocator> result;
    for (size_t i = 0; i < Columns; ++i) {
        result[i] = s | m[i];
    }
    return result;
}

template<size_t Columns, size_t Rows, typename Type, typename Representation, typename Allocator>
    requires rush::HasBitXor<Type>
inline rush::Mat<Columns, Rows, Type, Representation, Allocator> operator^(
    const Type& s,
    const rush::Mat<Columns, Rows, Type, Representation, Allocator>& m) {
    rush::Mat<Columns, Rows, Type, Representation, Allocator> result;
    for (size_t i = 0; i < Columns; ++i) {
        result[i] = s ^ m[i];
    }
    return result;
}

template<size_t Columns, size_t Rows, typename Type, typename Representation, typename Allocator>
    requires rush::HasAnd<Type>
inline rush::Mat<Columns, Rows, Type, Representation, Allocator> operator&&(
    const Type& s,
    const rush::Mat<Columns, Rows, Type, Representation, Allocator>& m) {
    rush::Mat<Columns, Rows, Type, Representation, Allocator> result;
    for (size_t i = 0; i < Columns; ++i) {
        result[i] = s && m[i];
    }
    return result;
}

template<size_t Columns, size_t Rows, typename Type, typename Representation, typename Allocator>
    requires rush::HasOr<Type>
inline rush::Mat<Columns, Rows, Type, Representation, Allocator> operator||(
    const Type& s,
    const rush::Mat<Columns, Rows, Type, Representation, Allocator>& m) {
    rush::Mat<Columns, Rows, Type, Representation, Allocator> result;
    for (size_t i = 0; i < Columns; ++i) {
        result[i] = s || m[i];
    }
    return result;
}

// ENDREGION

#endif //RUSH_MAT_EXTRA_H
