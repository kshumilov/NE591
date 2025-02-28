#ifndef LINALG_PJ_H
#define LINALG_PJ_H

#include <concepts>
#include <span>
#include <vector>

#include "methods/array.h"
#include "methods/optimize.h"
#include "methods/linalg/matrix.h"
#include "methods/linalg/utils/math.h"

#include "methods/linalg/Axb/utils.h"


template<std::floating_point DType, std::invocable<std::size_t, std::size_t> MatElem>
constexpr auto point_jacobi
(
    MatElem A,
    std::span<const DType> b,
    const FixedPointIterSettings<DType> settings = FixedPointIterSettings{}
) -> IterativeAxbResult<DType>
{
    std::vector<DType> x(b.size());
    std::vector<DType> x_next(b.size());

    auto g = [&](std::span<const DType> x_curr) constexpr -> std::span<DType>
    {
        for (std::size_t i{}; i < x_next.size(); ++i)
        {
            DType dot_prod{};

            // j in [0, i)
            for (std::size_t j{}; j < i; ++j)
            {
                dot_prod += A(i, j) * x_curr[j];
            }

            // skip j == i

            // j in [i + 1, A.cols())
            for (std::size_t j{ i + 1 }; j < x_curr.size(); ++j)
            {
                dot_prod += A(i, j) * x_curr[j];
            }

            x_next[i] = (b[i] - dot_prod) / A(i, i);
        }

        std::swap(x, x_next);

        return std::span{ x };
    };

    const auto iter_result = fixed_point_iteration<std::span<DType>>(
        g,
        x,
        max_rel_diff<std::span<const DType>, std::span<const DType>>,
        settings
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
constexpr auto point_jacobi
(
    const Matrix<DType>& A,
    std::span<const DType> b,
    const FixedPointIterSettings<DType> settings = FixedPointIterSettings{}
) -> IterativeAxbResult<DType>
{
    assert(not A.empty());
    assert(A.is_square());
    assert(A.rows() == b.size());
    assert(is_diag_nonzero(A));

    auto matelem = [&A](const std::size_t i, const std::size_t j) -> DType
    {
        return A[i, j];
    };

    return point_jacobi<DType>(matelem, b, settings);
}


template<std::floating_point DType>
constexpr auto point_jacobi
(
    const std::pair<Matrix<DType>, std::vector<DType>>& linear_system,
    const FixedPointIterSettings<DType> settings = FixedPointIterSettings{}
) -> IterativeAxbResult<DType>
{
    return point_jacobi<DType>(linear_system.first, linear_system.second, settings);
}


#endif // LINALG_PJ_H
