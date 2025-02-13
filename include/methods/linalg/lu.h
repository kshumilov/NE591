#ifndef LINALG_LU_H
#define LINALG_LU_H

#include <algorithm>  // min
#include <concepts>  // floating_point
#include <cstddef>  // size_t, ptrdiff_t
// #include <stdexcept> // invalid_argument
#include <span>
#include <utility>  // pair, unreachable
#include <tuple>
#include <vector>

#include "fmt/format.h"

#include "methods/linalg/matrix.h"


enum class LUResult {
    Success,
    SmallPivotEncountered,
};


enum class PivotingMethod {
    NoPivoting,
    PartialPivoting,
};


template<>
struct fmt::formatter<PivotingMethod, char> {
    template<class ParseContext>
    constexpr typename ParseContext::iterator parse(ParseContext& ctx)
    {
        return ctx.begin();
    }

    template<class FmtContext>
    constexpr auto format(const PivotingMethod q, FmtContext& ctx) const
    {
        switch (q) {
            case PivotingMethod::NoPivoting:
                return fmt::format_to(ctx.out(), "No Pivoting");
            case PivotingMethod::PartialPivoting:
                return fmt::format_to(ctx.out(), "Partial Pivoting");
            default:
                std::unreachable();
        }
        return ctx.out();
    }
};


template<std::floating_point scalar_t>
constexpr void lu_factor_inplace_update(Matrix<scalar_t>& A, const std::size_t k, LUResult& result, const scalar_t tol)
{
    assert(k < A.rows() and k < A.cols());

    for (std::size_t i{k + 1U}; i < A.rows(); ++i) {
        if (A(k, k) < tol) {
            result = LUResult::SmallPivotEncountered;
        }
        A(i, k) /= A(k, k);
        for (std::size_t j{k + 1U}; j < A.cols(); ++j) {
            A(i, j) -= A(i, k) * A(k, j);
        }
    }
}

template<std::floating_point scalar_t>
constexpr LUResult lu_factor_inplace(Matrix<scalar_t>& A, const scalar_t tol = scalar_t{1.0e-8})
{
    const auto n{std::min(A.rows(), A.cols())};

    auto result = LUResult::Success;

    for (std::size_t k{0U}; k < n - 1U; ++k) {
        lu_factor_inplace_update(A, k, result, tol);
    }

    return result;
}


template<std::floating_point scalar_t>
[[nodiscard]] constexpr
std::pair<Matrix<scalar_t>, LUResult> lup_factor_inplace(Matrix<scalar_t>& A, const scalar_t tol = scalar_t{1.0e-8})
{
    const auto n{std::min(A.rows(), A.cols())};

    auto result = LUResult::Success;

    std::vector<std::size_t> row_perm(A.rows());
    std::iota(row_perm.begin(), row_perm.end(), 0U);

    auto find_pivot = [&A](const std::size_t k) constexpr -> std::size_t {
        std::size_t pivot{k};
        for (std::size_t i{pivot + 1U}; i < A.rows(); ++i) {
            if (std::abs(A(i, k)) > std::abs(A(pivot, k)))
                pivot = i;
        }
        return pivot;
    };

    for (std::size_t k{0U}; k < n - 1U; ++k) {
        const auto pivot_row = find_pivot(k);

        std::swap(row_perm[k], row_perm[pivot_row]);
        A.swaprows(k, pivot_row);

        lu_factor_inplace_update(A, k, result, tol);
    }

    return std::make_pair(Matrix<scalar_t>::from_permutation(row_perm), result);
}


template<std::floating_point scalar_t>
[[nodiscard]] constexpr
Matrix<scalar_t> separate_lu(Matrix<scalar_t>& LU)
{
    auto L = Matrix<scalar_t>::eye(LU.rows(), LU.cols());

    for (const auto i: std::views::iota(0U, LU.rows())) {
        for (const auto j: std::views::iota(0U, i)) {
            std::swap(L(i, j), LU(i, j));
        }
    }

    return L;
}


template<std::floating_point scalar_t>
[[nodiscard]] constexpr
auto lu_factor(Matrix<scalar_t> A) -> std::tuple<Matrix<scalar_t>, Matrix<scalar_t>, LUResult>
{
    const auto result = lu_factor_inplace<scalar_t>(A);
    return std::make_tuple(separate_lu<scalar_t>(A), A, result);
}


template<std::floating_point scalar_t>
[[nodiscard]] constexpr
auto lup_factor(Matrix<scalar_t> A) -> std::tuple<Matrix<scalar_t>, Matrix<scalar_t>, Matrix<scalar_t>, LUResult>
{
    const auto& [P, result] = lup_factor_inplace<scalar_t>(A);
    return std::make_tuple(separate_lu<scalar_t>(A), A, P, result);
}


template<std::floating_point scalar_t, Diag LowerDiag = Diag::NonUnit>
[[nodiscard]] constexpr
std::vector<scalar_t> forward_substitution(const Matrix<scalar_t>& L, std::span<const scalar_t> b)
{
    assert(L.is_square());
    assert(L.rows() == b.size());

    // if (L.rows() != b.size()) {
    //     throw std::invalid_argument("Number of rows L does not match the size of b");
    // }

    std::vector<scalar_t> x(L.cols());

    for (std::size_t i{}; i < L.rows(); ++i) {
        x[i] = b[i];
        for (std::size_t j{}; j < i; j++)
            x[i] -= L(i, j) * x[j];

        if constexpr (LowerDiag == Diag::NonUnit) {
            x[i] /= L(i, i);
        }
    }

    return x;
}


template<std::floating_point scalar_t>
[[nodiscard]] constexpr
std::vector<scalar_t> backward_substitution(const Matrix<scalar_t>& U, std::span<const scalar_t> b)
{
    assert(U.is_square());
    assert(U.rows() == b.size());

    // if (U.rows() != b.size()) {
    //     throw std::invalid_argument("Number of rows U does not match the size of b");
    // }

    std::vector<scalar_t> x(U.cols());

    for (std::ptrdiff_t i{static_cast<std::ptrdiff_t>(U.rows()) - 1}; i >= 0; --i) {
        x[i] = b[i];
        for (std::size_t j{static_cast<std::size_t>(i) + 1U}; j < U.cols(); ++j)
            x[i] -= U(i, j) * x[j];

        x[i] /= U(i, i);
    }

    return x;
}


template<std::floating_point scalar_t, Diag LowerDiag = Diag::NonUnit>
[[nodiscard]] constexpr
std::vector<scalar_t> lu_solve(const Matrix<scalar_t>& L, const Matrix<scalar_t>& U, std::span<const scalar_t> b)
{
    assert(L.is_square());
    assert(U.is_square());
    assert(L.cols() == U.rows());
    assert(U.rows() == b.size());
    assert(L.rows() == b.size());

    // if (L.cols() != U.rows()) {
    //     throw std::invalid_argument(
    //         fmt::format("Incompatible shapes: L({}, {}) and U({}, {})", L.rows(), L.cols(), U.rows(), U.cols())
    //     );
    // }

    const auto y = forward_substitution<scalar_t, LowerDiag>(L, b);
    return backward_substitution<scalar_t>(U, y);
}

template<std::floating_point scalar_t>
[[nodiscard]] constexpr
std::vector<scalar_t> lu_solve(const Matrix<scalar_t>& LU, std::span<const scalar_t> b)
{
    return lu_solve<scalar_t, Diag::Unit>(LU, LU, b);
}


template<std::floating_point scalar_t, Diag LowerDiag = Diag::NonUnit>
[[nodiscard]] constexpr
std::vector<scalar_t> lup_solve(const Matrix<scalar_t>& L, const Matrix<scalar_t>& U, const Matrix<scalar_t>& P, std::span<const scalar_t> b)
{
    assert(P.is_square());
    assert(P.cols() == b.size());

    const auto z = P * b;
    return lu_solve<scalar_t, LowerDiag>(L, U, z);
}


template<std::floating_point scalar_t>
[[nodiscard]] constexpr
std::vector<scalar_t> lup_solve(const Matrix<scalar_t>& LU, const Matrix<scalar_t>& P, std::span<const scalar_t> b)
{
    return lup_solve<scalar_t, Diag::Unit>(LU, LU, P, b);
}

#endif //LINALG_LU_H
