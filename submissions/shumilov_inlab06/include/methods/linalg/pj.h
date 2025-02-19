#ifndef LINALG_PJ_H
#define LINALG_PJ_H

#include <iostream>
#include <concepts>
#include <tuple>
#include <vector>
#include <span>

#include <fmt/ostream.h>
#include <methods/array.h>

#include "methods/linalg/matrix.h"
#include "methods/linalg/blas.h"
#include "methods/linalg/utils/math.h"

#include "methods/optimize.h"

template<std::floating_point scalar_t>
struct PointJacobiResult {
    std::vector<scalar_t> x{};
    std::vector<scalar_t> residual{};
    bool converged{};
    int iters{};

    [[nodiscard]]
    constexpr auto to_string() const -> std::string {
        if (converged) {
            return fmt::format("Converged at iteration #{:d}: {:12.6e}", iters, error());
        }
        return fmt::format("Failed to converge in {:d} iterations: {:12.6e}", iters, error());
    }

    [[nodiscard]]
    const auto error() const -> scalar_t {
        return max_abs(residual);
    }
};

template<std::floating_point scalar_t>
constexpr auto point_jacobi(
        const Matrix<scalar_t>& R,
        std::span<const scalar_t> diag_inv,
        std::span<const scalar_t> b,
        const FixedPointIterSettings& settings = FixedPointIterSettings{}
) -> PointJacobiResult<scalar_t>
{
    assert(not R.empty());
    assert(R.is_square());
    assert(R.rows() == b.size());
    assert(R.rows() == diag_inv.size());

    std::vector<scalar_t> x_curr(R.cols());
    std::vector<scalar_t> x_next(R.cols());

    auto g = [&](std::span<const scalar_t> x) constexpr -> std::span<scalar_t> {
        std::copy(b.cbegin(), b.cend(), x_next.begin());

        // x_next = b - R * x
        gemv<scalar_t>(R, x, x_next, scalar_t{-1});

        // x_next = D^{-1} * x_next
        for (std::size_t i = 0; i < x_next.size(); ++i) {
            x_next[i] *= diag_inv[i];
        }

        std::swap(x_curr, x_next);

        return std::span{x_curr};
    };

    const auto result = fixed_point_iteration<std::span<scalar_t>>(
        g
      , x_curr
      , max_abs_diff<std::span<const scalar_t>>
      , settings
    );

    auto residual = get_residual<scalar_t>(R, result.x, b);
    for (std::size_t i = 0; i < residual.size(); ++i) {
       residual[i] -= result.x[i] / diag_inv[i];
    }

    return PointJacobiResult<scalar_t>{
        .x = std::move(x_curr)
      , .residual = std::move(residual)
      , .converged = result.converged
      , .iters = result.iters
    };
}

template<std::floating_point scalar_t>
constexpr auto point_jacobi(
        Matrix<scalar_t>& A,
        std::span<const scalar_t> b,
        const FixedPointIterSettings& settings = FixedPointIterSettings{}
) -> PointJacobiResult<scalar_t> {
    assert(not A.empty());
    assert(A.is_square());
    assert(A.rows() == b.size());

    auto diag_inv = extract_diagonal_inplace<scalar_t>(A);

    std::transform(
        diag_inv.cbegin()
      , diag_inv.cend()
      , diag_inv.begin()
      , [](const scalar_t a) { return 1 / a; }
    );

    const auto result = point_jacobi<scalar_t>(A, diag_inv, b, settings);

    // Return diagonal back into A
    for (std::size_t i = 0; i < A.rows(); ++i) {
        A[i, i] = 1 / diag_inv[i];
    }

    return result;
}


#endif //LINALG_PJ_H
