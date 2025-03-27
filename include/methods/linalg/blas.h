#ifndef LINALG_BLAS_H
#define LINALG_BLAS_H

#include <cassert>
#include <cmath>
#include <concepts>
#include <cstdlib>
#include <numeric>
#include <ranges>
#include <span>


template<std::floating_point T>
class Matrix;


enum class MatrixSymmetry : char
{
    Upper     = 'U',
    Lower     = 'L',
    Symmetric = 'S',
    Diagonal  = 'D',
    // Hermitian = 'H',
    General = 'G',
};

enum class MatrixOperation : char {
    Identity = 'I',
    Transpose = 'T'
};


enum class Diag : char
{
    NonUnit = 'N', Unit = 'U', Skip = 'S'
};


// x <- alpha * x
template<std::floating_point T>
void scal(std::span<T> x, const T alpha = T{ 1 }) noexcept
{
    for (std::size_t i{}; i < x.size(); ++i)
    {
        x[i] *= alpha;
    }
}


// y <- alpha * x + y
template<std::floating_point T>
void axpy(std::span<const T> x, std::span<T> y, const T alpha = T{ 1 }) noexcept
{
    assert(y.size() == x.size());
    // for (std::size_t i{}; i < x.size(); ++i)
    // {
    //     y[i] += alpha * x[i];
    // }

    std::transform(
        x.cbegin(), x.cend(),
        y.cbegin(), y.begin(),
        [&](const auto x_i, const auto y_i)
        {
            return alpha * x_i + y_i;
        }
    );
}


auto dot(const std::ranges::range auto& lhs, const std::ranges::range auto& rhs)
{
    assert(lhs.size() == rhs.size());
    return std::transform_reduce(
        std::cbegin(lhs),
        std::cend(lhs),
        std::cbegin(rhs),
        std::ranges::range_value_t<decltype(lhs)>{}
    );
}


auto norm_l2(const std::ranges::range auto& v) { return std::sqrt(dot(v, v)); }


template<
    std::floating_point DType,
    MatrixSymmetry symm = MatrixSymmetry::General,
    Diag diag = Diag::NonUnit,
    MatrixOperation op = MatrixOperation::Identity,
    std::invocable<std::size_t, std::size_t> MatElem
>
void gemv
(
    MatElem A,
    const std::size_t rows,
    const std::size_t cols,
    std::span<const DType> x,
    std::span<DType> y,
    const DType alpha = DType{ 1 },
    const DType beta = DType{}
) noexcept
{
    assert(rows == y.size());
    assert(cols == x.size());

    const auto zero = DType{};
    const auto one = DType{ 1 };

    if (rows * cols == std::size_t{} or (alpha == zero and beta == one))
        return;

    // Form y = beta * y
    if (beta != one)
    {
        if (beta == zero)
            std::fill(y.begin(), y.end(), zero);
        else
            for (std::size_t i{}; i < y.size(); ++i)
                y[i] *= beta;
    }

    if (alpha == zero)
        return;

    auto kernel = [&](const std::size_t i, const std::size_t j) constexpr -> DType
    {
        return A(i, j) * x[j];
    };

    for (std::size_t i{}; i < rows; ++i)
    {
        DType row_dot_x{};
        if constexpr (symm == MatrixSymmetry::Upper)
        {
            if constexpr (diag == Diag::NonUnit)
                row_dot_x += kernel(i, i);
            else if constexpr (diag == Diag::Unit)
                row_dot_x += x[i];

            for (std::size_t j{ i + 1U }; j < cols; ++j)
                row_dot_x += kernel(i, j);
        }
        else if constexpr (symm == MatrixSymmetry::Lower)
        {
            for (std::size_t j{}; j < i; ++j)
                row_dot_x += kernel(i, j);

            if constexpr (diag == Diag::NonUnit)
                row_dot_x += kernel(i, i);
            else if constexpr (diag == Diag::Unit)
                row_dot_x += x[i];
        }
        else if constexpr (symm == MatrixSymmetry::Diagonal)
        {
            if constexpr (diag == Diag::NonUnit)
                row_dot_x += kernel(i, i);
            else if constexpr (diag == Diag::Unit)
                row_dot_x += x[i];
        }
        else
        {
            if constexpr (diag == Diag::NonUnit)
                for (std::size_t j{}; j < cols; ++j)
                    row_dot_x += kernel(i, j);
            else
            {
                for (std::size_t j{}; j < i; ++j)
                    row_dot_x += kernel(i, j);

                if constexpr (diag == Diag::Unit)
                    row_dot_x += x[i];

                for (std::size_t j{ i + 1 }; j < A.cols(); ++j)
                    row_dot_x += kernel(i, j);
            }
        }

        y[i] += alpha * row_dot_x;
    }
}


// y <- alpha * A * x + beta * y
template<
    std::floating_point DType,
    MatrixSymmetry symm = MatrixSymmetry::General,
    Diag diag = Diag::NonUnit,
    MatrixOperation op = MatrixOperation::Identity
>
void gemv
(
    const Matrix<DType>& A,
    std::span<const DType> x,
    std::span<DType> y,
    const DType alpha = DType{ 1 },
    const DType beta = DType{}
) noexcept
{
    auto matelem = [&](const std::size_t i, const std::size_t j) constexpr -> DType
    {
        return A[i, j];
    };

    gemv<DType, symm, diag, op>(matelem, A.rows(), A.cols(), x, y, alpha, beta);
}


// C <- alpha * A * B + beta * C
template<std::floating_point scalar_t>
void gemm
(
    const Matrix<scalar_t>& A,
    const Matrix<scalar_t>& B,
    Matrix<scalar_t>& C,
    const scalar_t alpha = scalar_t{ 1 },
    const scalar_t beta = scalar_t{ 1 }
) noexcept
{
    assert(C.rows() == A.rows());
    assert(C.cols() == B.cols());
    assert(A.cols() == B.rows());

    for (std::size_t i{}; i < C.rows(); ++i)
    {
        for (std::size_t j{}; j < C.cols(); ++j)
        {
            scalar_t tmp{};
            for (std::size_t k{}; k < A.cols(); ++k)
            {
                tmp += A[i, k] * B[k, j];
            }
            C[i, j] *= alpha * tmp + beta * C[i, j];
        }
    }
}

#endif // LINALG_BLAS_H
