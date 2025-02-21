#ifndef LINALG_AXB_GS_H
#define LINALG_AXB_GS_H

#include <concepts>
#include <vector>
#include <span>

#include "methods/array.h"
#include "methods/linalg/matrix.h"
#include "methods/optimize.h"

#include "methods/linalg/Axb/utils.h"


template<std::floating_point DType>
constexpr auto gauss_seidel(
        const Matrix<DType>& A,
        std::span<const DType> b,
        const FixedPointIterSettings<DType> settings = FixedPointIterSettings{}
) -> IterativeAxbResult<DType>
{
    assert(not A.empty());
    assert(A.is_square());
    assert(A.rows() == b.size());
    assert(is_diag_nonzero(A));

    std::vector<DType> x(A.cols());
    std::vector<DType> Ax(b.size());

    auto g = [&](std::span<DType> x_curr) constexpr -> std::span<DType> {
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

            x_curr[i] = (b[i] - dot_prod) / A[i, i];
        }

        return x_curr;
    };

    auto error = [&](std::span<const DType> x_curr) constexpr -> DType {
        gemv<DType>(A, x_curr, Ax);
        return max_abs_diff(Ax, b);
    };

    const auto iter_result = fixed_point_iteration<std::span<DType>>(g, x, error, settings);

    return IterativeAxbResult<DType>{
        .x = std::move(x),
        .error = iter_result.error,
        .converged = iter_result.converged,
        .iters = iter_result.iters
    };
}


template<std::floating_point DType>
constexpr auto gauss_seidel(
        const std::pair<Matrix<DType>, std::vector<DType>>& linear_system,
        const FixedPointIterSettings<DType> settings = FixedPointIterSettings{}
) -> IterativeAxbResult<DType> {
    return gauss_seidel<DType>(linear_system.first, linear_system.second, settings);
}

#endif // LINALG_AXB_GS_H