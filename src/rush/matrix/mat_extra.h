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
        size_t h = 0;
        for (int c = 0; c < C; ++c) {
            for (int r = 0; r < R; ++r) {
              h ^= hasher( s( c, r)) + 0x9e3779b9
                   + (h << 6) + (h >> 2);
            }
        }
        return h;
    }
};

// REGION EXTRA OPERATIONS

template<size_t Columns, size_t Rows, typename Type>
requires rush::HasAdd<Type>
inline rush::Mat<Columns, Rows, Type> operator+(
        const Type& s,
        const rush::Mat<Columns, Rows, Type>& m) {
    rush::Mat<Columns, Rows, Type> result;
    for (size_t i = 0; i < Columns; ++i) {
        result[i] = s + m[i];
    }
    return result;
}

template<size_t Columns, size_t Rows, typename Type>
requires rush::HasSub<Type>
inline rush::Mat<Columns, Rows, Type> operator-(
        const Type& s,
        const rush::Mat<Columns, Rows, Type>& m) {
    rush::Mat<Columns, Rows, Type> result;
    for (size_t i = 0; i < Columns; ++i) {
        result[i] = s - m[i];
    }
    return result;
}

template<size_t Columns, size_t Rows, typename Type>
requires rush::HasMul<Type>
inline rush::Mat<Columns, Rows, Type> operator*(
        const Type& s,
        const rush::Mat<Columns, Rows, Type>& m) {
    rush::Mat<Columns, Rows, Type> result;
    for (size_t i = 0; i < Columns; ++i) {
        result[i] = s * m[i];
    }
    return result;
}

template<size_t Columns, size_t Rows, typename Type>
requires rush::HasDiv<Type>
inline rush::Mat<Columns, Rows, Type> operator/(
        const Type& s,
        const rush::Mat<Columns, Rows, Type>& m) {
    rush::Mat<Columns, Rows, Type> result;
    for (size_t i = 0; i < Columns; ++i) {
        result[i] = s / m[i];
    }
    return result;
}

template<size_t Columns, size_t Rows, typename Type>
requires rush::HasShl<Type>
inline rush::Mat<Columns, Rows, Type> operator<<(
        const Type& s,
        const rush::Mat<Columns, Rows, Type>& m) {
    rush::Mat<Columns, Rows, Type> result;
    for (size_t i = 0; i < Columns; ++i) {
        result[i] = s << m[i];
    }
    return result;
}

template<size_t Columns, size_t Rows, typename Type>
requires rush::HasShr<Type>
inline rush::Mat<Columns, Rows, Type> operator>>(
        const Type& s,
        const rush::Mat<Columns, Rows, Type>& m) {
    rush::Mat<Columns, Rows, Type> result;
    for (size_t i = 0; i < Columns; ++i) {
        result[i] = s >> m[i];
    }
    return result;
}

template<size_t Columns, size_t Rows, typename Type>
requires rush::HasBitAnd<Type>
inline rush::Mat<Columns, Rows, Type> operator&(
        const Type& s,
        const rush::Mat<Columns, Rows, Type>& m) {
    rush::Mat<Columns, Rows, Type> result;
    for (size_t i = 0; i < Columns; ++i) {
        result[i] = s & m[i];
    }
    return result;
}

template<size_t Columns, size_t Rows, typename Type>
requires rush::HasBitOr<Type>
inline rush::Mat<Columns, Rows, Type> operator|(
        const Type& s,
        const rush::Mat<Columns, Rows, Type>& m) {
    rush::Mat<Columns, Rows, Type> result;
    for (size_t i = 0; i < Columns; ++i) {
        result[i] = s | m[i];
    }
    return result;
}

template<size_t Columns, size_t Rows, typename Type>
requires rush::HasBitXor<Type>
inline rush::Mat<Columns, Rows, Type> operator^(
        const Type& s,
        const rush::Mat<Columns, Rows, Type>& m) {
    rush::Mat<Columns, Rows, Type> result;
    for (size_t i = 0; i < Columns; ++i) {
        result[i] = s ^ m[i];
    }
    return result;
}

template<size_t Columns, size_t Rows, typename Type>
requires rush::HasAnd<Type>
inline rush::Mat<Columns, Rows, Type> operator&&(
        const Type& s,
        const rush::Mat<Columns, Rows, Type>& m) {
    rush::Mat<Columns, Rows, Type> result;
    for (size_t i = 0; i < Columns; ++i) {
        result[i] = s && m[i];
    }
    return result;
}

template<size_t Columns, size_t Rows, typename Type>
requires rush::HasOr<Type>
inline rush::Mat<Columns, Rows, Type> operator||(
        const Type& s,
        const rush::Mat<Columns, Rows, Type>& m) {
    rush::Mat<Columns, Rows, Type> result;
    for (size_t i = 0; i < Columns; ++i) {
        result[i] = s || m[i];
    }
    return result;
}

// ENDREGION

#endif //RUSH_MAT_EXTRA_H
