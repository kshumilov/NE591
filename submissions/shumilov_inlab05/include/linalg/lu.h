#ifndef LINALG_LU_H
#define LINALG_LU_H

#include <algorithm>  // min
#include <concepts>  // floating_point
#include <cstddef>  // size_t, ptrdiff_t
#include <stdexcept> // invalid_argument

#include <span>
#include <utility>  // pair
#include <vector>

#include "linalg/matrix.h"


template<std::floating_point scalar_t>
constexpr auto lu_inplace(Matrix<scalar_t>& A) -> void
{
    if (A.empty())
        return;

    const auto n = std::min(A.rows(), A.cols());

    for (std::size_t i{}; i < n; i++) {
        // Compute upper triangular matrix U (stored in A)
        for (std::size_t k{i}; k < n; k++) {
            scalar_t sum{};
            for (std::size_t j{}; j < i; j++)
                //     L[i, j] * U[j, k]
                sum += A(i, j) * A(j, k);

            A(i, k) -= sum;  // Store U in A
        }

        // Compute lower triangular matrix L (stored in A, below diagonal)
        for (std::size_t k{i + 1U}; k < n; k++) {
            scalar_t sum{};
            for (std::size_t j{}; j < i; j++)
                //     L[k, j] * U[j, i]
                sum += A(k, j) * A(j, i);

            A(k, i) = (A(k, i) - sum) / A(i, i);  // Store L in A
        }
    }
}


template<std::floating_point scalar_t>
[[nodiscard]]
constexpr auto separate_lu(const Matrix<scalar_t>& LU) -> std::pair<Matrix<scalar_t>, Matrix<scalar_t>> {
    auto L = Matrix<scalar_t>::eye(LU.rows(), LU.cols());
    auto U = Matrix<scalar_t>::zeros_like(LU);

    for (const auto i : std::views::iota(0U, LU.rows())) {
        for (const auto j : std::views::iota(0U, LU.cols())) {
            if (i > j) {
                L(i, j) = LU(i, j);
            }
            else {
                U(i, j) = LU(i, j);
            }
        }
    }

   return std::make_pair(L, U);
}


template<std::floating_point scalar_t>
[[nodiscard]]
constexpr auto lu(Matrix<scalar_t> A) -> std::pair<Matrix<scalar_t>, Matrix<scalar_t>>
{
    lu_inplace<scalar_t>(A);
    return split_lu<scalar_t>(A);
}


template<std::floating_point scalar_t, Diag UnitLower = Diag::NonUnit>
[[nodiscard]]
constexpr auto forward_substitution(const Matrix<scalar_t>& L, std::span<const scalar_t> b) -> std::vector<scalar_t>
{
    if (L.rows() != b.size()) {
        throw std::invalid_argument("Number of rows L does not match the size of b");
    }

    std::vector<scalar_t> x(L.cols());

    if (x.empty())
        return x;

    for (std::size_t r{}; r < L.rows(); r++) {
        scalar_t sum_lx{};
        for (std::size_t j{}; j < r; j++)
            sum_lx += L(r, j) * x[j];

        x[r] = b[r] - sum_lx;

        if constexpr (UnitLower == Diag::NonUnit) {
            x[r] /= L(r, r);
        }
    }

    return x;
}


template<std::floating_point scalar_t>
auto backward_substitution(const Matrix<scalar_t>& U, std::span<const scalar_t> b) -> std::vector<scalar_t>
{
    if (U.rows() != b.size()) {
        throw std::invalid_argument("Number of rows U does not match the size of b");
    }

    std::vector<scalar_t> x(U.cols());

    if (x.empty())
        return x;

    for (std::ptrdiff_t r{static_cast<std::ptrdiff_t>(U.rows()) - 1}; r >=0; --r) {
        // \sum_{j=r+1}^{n} u_{rj} * x_{j}
        scalar_t sum_ux{};
        for (std::ptrdiff_t j{r + 1}; j < U.cols(); ++j) {
            sum_ux += U(r, j) * x[j];
        }
        x[r] = (b[r] - sum_ux) / U(r, r);
    }

    return x;
}


template<std::floating_point scalar_t, Diag UnitLower = Diag::NonUnit>
constexpr auto solve_lu(const Matrix<scalar_t>& L, const Matrix<scalar_t>& U, std::span<const scalar_t> b) -> std::vector<scalar_t>
{
    if (L.cols() != U.rows()) {
        throw std::invalid_argument(
            fmt::format("Incompatible shapes: L({}, {}) and U({}, {})", L.rows(), L.cols(), U.rows(), U.cols())
        );
    }

    const auto y = forward_substitution<scalar_t, UnitLower>(L, b);
    return backward_substitution<scalar_t>(U, y);
}


template<std::floating_point scalar_t, Diag UnitLower = Diag::NonUnit>
auto solve_lup(const Matrix<scalar_t>& L, const Matrix<scalar_t>& U, const Matrix<scalar_t>& P, std::span<const scalar_t> b) -> std::vector<scalar_t>
{
    const auto z = P * b;
    return solve_lu<scalar_t, UnitLower>(L, U, z);
}

#endif //LINALG_LU_H
