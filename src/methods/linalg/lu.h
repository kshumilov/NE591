#ifndef LU_H
#define LU_H

#include <concepts>
#include <ranges>
#include <span>
#include <vector>
#include <tuple>
#include <functional>

#include "linalg/matrix.h"


template<std::floating_point scalar_t>
auto forward_substitution(const Matrix<scalar_t>& L, std::span<const scalar_t> b) -> std::vector<scalar_t>
{
    assert(L.rows() == b.size());

    std::vector<scalar_t> x(L.cols());

    for (const auto r: std::views::iota(0U, L.rows())) {
        // \sum_{j=0}^{r} l_{rj} * x_{j}
        const auto sum_lx_ = std::ranges::fold_left(
            std::views::zip(x, L.row_view(r))
            | std::views::take(r)
            | std::views::transform(
                [](const auto& v) {
                    return std::get<0>(v) * std::get<1>(v);
                })
            | std::views::as_const,
            0.0, std::plus<scalar_t>{}
        );

        x[r] = (b[r] - sum_lx_) / L(r, r);
    }

    return x;
}


template<std::floating_point scalar_t>
auto backward_substitution(const Matrix<scalar_t>& U, std::span<const scalar_t> b) -> std::vector<scalar_t>
{
    // assert(U.is_square());
    assert(U.rows() == b.size());

    std::vector<scalar_t> x(U.cols());

    for (const auto r : std::views::iota(0U, U.rows()) | std::views::reverse) {
        // \sum_{j=r}^{n} u_{rj} * x_{j}
        const auto sum_ux = std::ranges::fold_left(
            std::views::zip(x, U.row_view(r))
            | std::views::drop(r + 1)
            | std::views::transform(
                [](const auto& v) {
                    return std::get<0>(v) * std::get<1>(v);
                })
            | std::views::as_const,
            0.0, std::plus<scalar_t>{}
        );

        x[r] = (b[r] - sum_ux) / U(r, r);
    }

    return x;
}


template<std::floating_point scalar_t>
auto solve_lu(const Matrix<scalar_t>& L, const Matrix<scalar_t>& U, std::span<const scalar_t> b) -> std::vector<scalar_t>
{
    assert(L.rows() == b.size());
    assert(L.cols() == U.rows());

    const auto y = forward_substitution<scalar_t>(L, b);
    return backward_substitution<scalar_t>(U, y);
}

#endif //LU_H
