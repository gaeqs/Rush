//
// Created by gaelr on 03/08/2023.
//

#ifndef RUSH_MAT_H
#define RUSH_MAT_H

#include <rush/matrix/mat_dense_rep.h>
#include <rush/matrix/mat_sparse_rep.h>

#include <rush/matrix/mat_base.h>
#include <rush/matrix/mat_extra.h>

namespace rush {
    using Mat1f = rush::Mat<1, 1, float>;
    using Mat2f = rush::Mat<2, 2, float>;
    using Mat3f = rush::Mat<3, 3, float>;
    using Mat4f = rush::Mat<4, 4, float>;

    using Mat1d = rush::Mat<1, 1, double>;
    using Mat2d = rush::Mat<2, 2, double>;
    using Mat3d = rush::Mat<3, 3, double>;
    using Mat4d = rush::Mat<4, 4, double>;

    using Mat1i = rush::Mat<1, 1, int32_t>;
    using Mat2i = rush::Mat<2, 2, int32_t>;
    using Mat3i = rush::Mat<3, 3, int32_t>;
    using Mat4i = rush::Mat<4, 4, int32_t>;

    using Mat1l = rush::Mat<1, 1, int64_t>;
    using Mat2l = rush::Mat<2, 2, int64_t>;
    using Mat3l = rush::Mat<3, 3, int64_t>;
    using Mat4l = rush::Mat<4, 4, int64_t>;

    using SparseMat1f = rush::Mat<1, 1, float, MatSparseRep>;
    using SparseMat2f = rush::Mat<2, 2, float, MatSparseRep>;
    using SparseMat3f = rush::Mat<3, 3, float, MatSparseRep>;
    using SparseMat4f = rush::Mat<4, 4, float, MatSparseRep>;

    using SparseMat1d = rush::Mat<1, 1, double, MatSparseRep>;
    using SparseMat2d = rush::Mat<2, 2, double, MatSparseRep>;
    using SparseMat3d = rush::Mat<3, 3, double, MatSparseRep>;
    using SparseMat4d = rush::Mat<4, 4, double, MatSparseRep>;

    using SparseMat1i = rush::Mat<1, 1, int32_t, MatSparseRep>;
    using SparseMat2i = rush::Mat<2, 2, int32_t, MatSparseRep>;
    using SparseMat3i = rush::Mat<3, 3, int32_t, MatSparseRep>;
    using SparseMat4i = rush::Mat<4, 4, int32_t, MatSparseRep>;

    using SparseMat1l = rush::Mat<1, 1, int64_t, MatSparseRep>;
    using SparseMat2l = rush::Mat<2, 2, int64_t, MatSparseRep>;
    using SparseMat3l = rush::Mat<3, 3, int64_t, MatSparseRep>;
    using SparseMat4l = rush::Mat<4, 4, int64_t, MatSparseRep>;
}

#endif //RUSH_MAT_H
