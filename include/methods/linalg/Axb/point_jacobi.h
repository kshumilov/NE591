#ifndef LINALG_PJ_H
#define LINALG_PJ_H

#include <concepts>
#include <vector>
#include <span>

#include "methods/array.h"
#include "methods/linalg/blas.h"
#include "methods/linalg/utils/math.h"
#include "methods/linalg/matrix.h"
#include "methods/optimize.h"

#include "methods/linalg/Axb/utils.h"


template<std::floating_point DType>
constexpr auto point_jacobi(
        const Matrix<DType>& A,
        std::span<const DType> b,
        const FixedPointIterSettings<DType> settings = FixedPointIterSettings{}
) -> IterativeAxbResult<DType>
{
    assert(not A.empty());
    assert(A.is_square());
    assert(A.rows() == b.size());
    assert(is_diag_nonzero(A));

    std::vector<DType> x(A.rows());
    std::vector<DType> tmp(A.rows());
    std::vector<DType> residual(A.rows());

    auto g = [&](std::span<const DType> x_curr) constexpr -> std::span<DType> {
        for (std::size_t i{}; i < A.rows(); ++i) {
            DType dot_prod{};

            // j in [0, i)
            for (std::size_t j{}; j < i; ++j) {
                dot_prod += A[i, j] * x_curr[j];
            }

            // skip j == i

            // j in [i + 1, A.cols())
            for (std::size_t j{i + 1}; j < A.cols(); ++j) {
                dot_prod += A[i, j] * x_curr[j];
            }

            tmp[i] = (b[i] - dot_prod) / A[i, i];
        }

        std::swap(x, tmp);

        return std::span{x};
    };

    auto error = [&](std::span<const DType> x_next, std::span<const DType>) constexpr -> DType {
        std::copy(b.cbegin(), b.cend(), residual.begin());
        build_residual_inplace<DType>(A, x_next, residual);
        return max_abs(residual);
    };

    const auto iter_result = fixed_point_iteration<std::span<DType>>(
       g, x, error, settings
    );

    return IterativeAxbResult<DType>{
        .x = std::move(x),
        .error = iter_result.error,
        .converged = iter_result.converged,
        .iters = iter_result.iters
    };
}


template<std::floating_point DType>
constexpr auto point_jacobi(
        const std::pair<Matrix<DType>, std::vector<DType>>& linear_system,
        const FixedPointIterSettings<DType> settings = FixedPointIterSettings{}
) -> IterativeAxbResult<DType> {
    return point_jacobi<DType>(linear_system.first, linear_system.second, settings);
}
#endif //LINALG_PJ_H
