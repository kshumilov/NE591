#ifndef LINALG_AXB_SOR_H
#define LINALG_AXB_SOR_H

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
constexpr auto successive_over_relaxation(
        const Matrix<DType>& A,
        std::span<const DType> b,
        const DType relaxation_factor,
        const FixedPointIterSettings<DType> settings = FixedPointIterSettings{}
) -> IterativeAxbResult<DType>
{
    assert(not A.empty());
    assert(A.is_square());
    assert(A.rows() == b.size());
    assert(is_diag_nonzero(A));
    assert(relaxation_factor > 1.0);

    std::vector<DType> x(A.rows());
    std::vector<DType> x_next(A.rows());
    // std::vector<DType> Ax(A.rows());

    auto g = [&](std::span<DType> x_curr) constexpr -> std::span<DType> {
        for (std::size_t i{}; i < A.rows(); ++i) {
            DType dot_prod{};

            // j in [0, i)
            for (std::size_t j{}; j < i; ++j) {
                dot_prod += A[i, j] * x_next[j];
            }

            // skip j == i

            // j in [i + 1, A.cols())
            for (std::size_t j{i + 1}; j < A.cols(); ++j) {
                dot_prod += A[i, j] * x_curr[j];
            }

            x_next[i] = (1 - relaxation_factor) * x_curr[i]
                      + relaxation_factor * (b[i] - dot_prod) / A[i, i];
        }

        std::swap(x, x_next);

        return std::span{x};
    };

    // auto error = [&](std::span<const DType> x_curr) constexpr -> DType {
    //     gemv<DType>(A, x_curr, Ax);
    //     return max_abs_diff(Ax, b);
    // };

    const auto iter_result = fixed_point_iteration<std::span<DType>>(
       g, x, max_rel_diff<std::span<const DType>, std::span<const DType>>, settings
    );

    const auto residual = get_residual<DType>(A, x, b);

    return IterativeAxbResult<DType>{
        .x = std::move(x),
        .relative_error = iter_result.error,
        .residual_error = max_abs(residual),
        .converged = iter_result.converged,
        .iters = iter_result.iters
    };
}


template<std::floating_point DType>
constexpr auto successive_over_relaxation(
        const std::pair<Matrix<DType>, std::vector<DType>>& linear_system,
        const DType relaxation_factor,
        const FixedPointIterSettings<DType> settings = FixedPointIterSettings{}
) -> IterativeAxbResult<DType> {
    return successive_over_relaxation<DType>(linear_system.first, linear_system.second, relaxation_factor, settings);
}

#endif // LINALG_AXB_SOR_H