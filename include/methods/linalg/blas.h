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
        std::cbegin(rhs), 0
    );
}


auto norm_l2(const std::ranges::range auto& v)
{
    return std::sqrt(dot(v, v));
}


// y <- alpha * A * x + beta * y
template<std::floating_point scalar_t>
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
        y[i] *= beta;
        for (std::size_t j{}; j < y.size(); ++j) {
            y[i] += alpha * A[i, j] * x[j];
        }
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
    for (std::size_t i{}; i < C.rows(); ++i) {
        for (std::size_t j{}; j < C.cols(); ++j) {
            C[i, j] *= beta;
            for (std::size_t k{}; k < A.cols(); ++k) {
                C[i, j] += alpha * A[i, k] * B[k, j];
            }
        }
    }
}

#endif // LINALG_BLAS_H
