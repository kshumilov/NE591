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
#include <string_view>

#include "fmt/format.h"

#include "methods/linalg/matrix.h"
#include "methods/linalg/utils/math.h"
#include "methods/math.h"


enum class LUResult
{
    Success, SmallPivotEncountered,
};


enum class PivotingMethod
{
    NoPivoting, PartialPivoting,
};


template<>
struct fmt::formatter<PivotingMethod>
{
    fmt::formatter<std::string_view> underlying_;

    constexpr auto parse(auto& ctx)
    {
        return underlying_.parse(ctx);
    }

    constexpr auto format(const PivotingMethod q, auto& ctx) const
    {
        const auto name = [&q] constexpr -> std::string_view
        {
            switch (q)
            {
                case PivotingMethod::NoPivoting:
                    return "No Pivoting";
                case PivotingMethod::PartialPivoting:
                    return "Partial Pivoting";
                default:
                    std::unreachable();
            }
        }();

        return underlying_.format(name, ctx);
    }
};


template<std::floating_point DType>
constexpr auto lu_factor_inplace_update(Matrix<DType>& A, const std::size_t k) -> bool
{
    assert(k < A.rows() and k < A.cols());

    const auto small_pivot_found{ isclose(A[k, k], DType{}) };

    for (std::size_t i{ k + 1U }; i < A.rows(); ++i)
    {
        A[i, k] /= A[k, k];
        for (std::size_t j{ k + 1U }; j < A.cols(); ++j)
        {
            A[i, j] -= A[i, k] * A[k, j];
        }
    }

    return small_pivot_found;
}


template<std::floating_point DType>
[[nodiscard]] constexpr
auto lu_factor_inplace(Matrix<DType>& A) -> LUResult
{
    assert(not A.empty());

    const auto n{ std::min(A.rows(), A.cols()) };

    auto small_pivot_found{ false };
    for (std::size_t k{}; k < n - 1U; ++k)
    {
        const auto small_pivot_k = lu_factor_inplace_update(A, k);
        small_pivot_found |= small_pivot_k;
    }

    return small_pivot_found ? LUResult::SmallPivotEncountered : LUResult::Success;
}


template<std::floating_point DType>
[[nodiscard]] constexpr
auto lup_factor_inplace(Matrix<DType>& A) -> std::pair<Matrix<DType>, LUResult>
{
    assert(not A.empty());

    const auto n{ std::min(A.rows(), A.cols()) };

    std::vector<std::size_t> row_perm(A.rows());
    std::iota(row_perm.begin(), row_perm.end(), 0U);

    auto small_pivot_found{ false };
    for (std::size_t k{}; k < n - 1U; ++k)
    {
        // Find Pivot
        std::size_t pivot{ k };
        for (std::size_t i{ pivot + 1U }; i < A.rows(); ++i)
        {
            if (std::abs(A[i, k]) > std::abs(A[pivot, k]))
                pivot = i;
        }

        if (pivot != k)
        {
            std::swap(row_perm[k], row_perm[pivot]);
            A.swaprows(k, pivot);
        }

        const auto small_pivot_k = lu_factor_inplace_update(A, k);
        small_pivot_found |= small_pivot_k;
    }

    return std::make_pair(
        Matrix<DType>::from_permutation(row_perm),
        small_pivot_found ? LUResult::SmallPivotEncountered : LUResult::Success
    );
}


template<std::floating_point DType>
[[nodiscard]] constexpr
auto lu_factor(Matrix<DType> A) -> std::tuple<Matrix<DType>, Matrix<DType>, LUResult>
{
    const auto result = lu_factor_inplace<DType>(A);
    return std::make_tuple(extract_lowerunit_inplace<DType>(A), A, result);
}


template<std::floating_point DType>
[[nodiscard]] constexpr
auto lup_factor(Matrix<DType> A) -> std::tuple<Matrix<DType>, Matrix<DType>, Matrix<DType>, LUResult>
{
    const auto& [P, result] = lup_factor_inplace<DType>(A);
    return std::make_tuple(extract_lowerunit_inplace<DType>(A), A, P, result);
}


template<std::floating_point DType, Diag LowerDiag = Diag::NonUnit>
[[nodiscard]] constexpr
auto forward_substitution(const Matrix<DType>& L, std::span<const DType> b) -> std::vector<DType>
{
    assert(L.is_square());
    assert(L.rows() == b.size());

    std::vector<DType> x(L.cols());

    for (std::size_t i{}; i < L.rows(); ++i)
    {
        x[i] = b[i];
        for (std::size_t j{}; j < i; j++)
            x[i] -= L[i, j] * x[j];

        if constexpr (LowerDiag == Diag::NonUnit)
            x[i] /= L[i, i];
    }

    return x;
}


template<std::floating_point DType>
[[nodiscard]] constexpr
auto backward_substitution(const Matrix<DType>& U, std::span<const DType> b) -> std::vector<DType>
{
    assert(U.is_square());
    assert(U.rows() == b.size());

    std::vector<DType> x(U.cols());

    for (std::ptrdiff_t i{ static_cast<std::ptrdiff_t>(U.rows()) - 1 }; i >= 0; --i)
    {
        x[i] = b[i];
        for (std::size_t j{ static_cast<std::size_t>(i) + 1U }; j < U.cols(); ++j)
            x[i] -= U[i, j] * x[j];

        x[i] /= U[i, i];
    }

    return x;
}


template<std::floating_point DType, Diag LowerDiag = Diag::NonUnit>
[[nodiscard]] constexpr
std::vector<DType> lu_solve(const Matrix<DType>& L, const Matrix<DType>& U, std::span<const DType> b)
{
    assert(L.is_square());
    assert(U.is_square());
    assert(L.cols() == U.rows());
    assert(U.rows() == b.size());
    assert(L.rows() == b.size());

    const auto y = forward_substitution<DType, LowerDiag>(L, b);
    return backward_substitution<DType>(U, y);
}


template<std::floating_point DType>
[[nodiscard]] constexpr
std::vector<DType> lu_solve(const Matrix<DType>& LU, std::span<const DType> b)
{
    return lu_solve<DType, Diag::Unit>(LU, LU, b);
}


template<std::floating_point DType, Diag LowerDiag = Diag::NonUnit>
[[nodiscard]] constexpr
auto lup_solve
(const Matrix<DType>& L, const Matrix<DType>& U, const Matrix<DType>& P, std::span<const DType> b) -> std::vector<DType>
{
    assert(P.is_square());
    assert(P.cols() == b.size());

    const auto z = P * b;
    return lu_solve<DType, LowerDiag>(L, U, z);
}


template<std::floating_point DType>
[[nodiscard]] constexpr
std::vector<DType> lup_solve(const Matrix<DType>& LU, const Matrix<DType>& P, std::span<const DType> b)
{
    return lup_solve<DType, Diag::Unit>(LU, LU, P, b);
}

#endif //LINALG_LU_H
