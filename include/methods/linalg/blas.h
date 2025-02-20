#ifndef LINALG_BLAS_H
#define LINALG_BLAS_H

#include <concepts>
#include <cstdlib>
#include <span>
#include <cassert>
#include <cmath>
#include <numeric>


template<std::floating_point T>
class Matrix;


enum class MatrixSymmetry : char {
    Upper   = 'U',
    Lower   = 'L',
    Symmetric = 'S',
    Diagonal = 'D',
    // Hermitian = 'H',
    General = 'G',
};


enum class Diag : char {
    NonUnit = 'N',
    Unit = 'U'
};


// x <- alpha * x
template<std::floating_point T>
void scal(std::span<T> x, const T alpha = T{1}) noexcept
{
    for (std::size_t i{}; i < x.size(); ++i) {
        x[i] *= alpha;
    }
}


// y <- alpha * x + y
template<std::floating_point T>
void axpy(std::span<const T> x, std::span<T> y, const T alpha = T{1}) noexcept
{
    assert(y.size() == x.size());
    for (std::size_t i{}; i < x.size(); ++i) {
        y[i] += alpha * x[i];
    }
}


auto dot(const std::ranges::range auto& lhs, const std::ranges::range auto& rhs)
{
    assert(lhs.size() == rhs.size());
    return std::transform_reduce(
        std::cbegin(lhs), std::cend(lhs),
        std::cbegin(rhs), std::ranges::range_value_t<decltype(lhs)>{}
    );
}


auto norm_l2(const std::ranges::range auto& v)
{
    return std::sqrt(dot(v, v));
}


// y <- alpha * A * x + beta * y
template<std::floating_point scalar_t, MatrixSymmetry symm = MatrixSymmetry::General, Diag diag = Diag::NonUnit>
void gemv(
    const Matrix<scalar_t>& A,
    std::span<const scalar_t> x,
    std::span<scalar_t> y,
    const scalar_t alpha = scalar_t{1},
    const scalar_t beta = scalar_t{1}
) noexcept
{
    assert(A.cols() == x.size());
    assert(y.size() == A.rows());

    for (std::size_t i{}; i < y.size(); ++i) {
        scalar_t tmp{};
        if constexpr (symm == MatrixSymmetry::Upper) {
            for (std::size_t j{i}; j < A.cols(); ++j) {
                tmp += A[i, j] * x[j];
            }
        }
        else if constexpr (symm == MatrixSymmetry::Lower) {
            for (std::size_t j{}; j < i; ++j) {
                tmp += A[i, j] * x[j];
            }

            if constexpr (diag == Diag::NonUnit) {
                tmp += A[i, i] * x[i];
            }
            else {
                tmp += x[i];
            }
        }
        else {
            for (std::size_t j{}; j < A.cols(); ++j) {
                tmp += A[i, j] * x[j];
            }
        }

        y[i] = alpha * tmp + beta * y[i];
    }
}

// C <- alpha * A * B + beta * C
template<std::floating_point scalar_t>
void gemm(
    const Matrix<scalar_t>& A,
    const Matrix<scalar_t>& B,
    Matrix<scalar_t>& C,
    const scalar_t alpha = scalar_t{1},
    const scalar_t beta = scalar_t{1}
) noexcept
{
    assert(C.rows() == A.rows());
    assert(C.cols() == B.cols());
    assert(A.cols() == B.rows());

    for (std::size_t i{}; i < C.rows(); ++i) {
        for (std::size_t j{}; j < C.cols(); ++j) {
            scalar_t tmp{};
            for (std::size_t k{}; k < A.cols(); ++k) {
                tmp += A[i, k] * B[k, j];
            }
            C[i, j] *= alpha * tmp + beta * C[i, j];
        }
    }
}

#endif // LINALG_BLAS_H
