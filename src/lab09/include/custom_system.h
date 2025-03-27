#ifndef GENERATE_MATRIX_H
#define GENERATE_MATRIX_H

#include <cstddef>
#include <concepts>
#include <utility>
#include <vector>
#include <algorithm>
#include <random>

#include "methods/linalg/matrix.h"

template<std::floating_point DType>
[[nodiscard]]
auto build_system(const std::size_t n) -> std::pair<Matrix<DType>, std::vector<DType>> {
    std::vector<DType> b(n);
    std::ranges::fill(b, DType{1});

    auto A = Matrix<DType>::from_func(n, [&](auto i, auto j) {
        if (i != j) {
            return DType{-1} / (static_cast<DType>(i + 1) + static_cast<DType>(j + 1));
        }
        return DType{};
    });

    for (std::size_t i{}; i < n; ++i) {
        A[i, i] = DType{1} / static_cast<DType>(n)
                - std::ranges::fold_left(A.row(i), DType{}, std::plus<DType>{});
    }

    return std::make_pair(std::move(A), std::move(b));
}


template<std::floating_point DType, MatrixSymmetry symm = MatrixSymmetry::General, Diag diag = Diag::NonUnit>
[[nodiscard]]
auto build_random_system(const int n, const DType lb = DType{}, const DType ub = DType{1}) -> std::pair<Matrix<DType>, std::vector<DType>>
{
    std::default_random_engine rng{};
    std::uniform_real_distribution<DType> unif{ lb, ub };
    auto A = Matrix<DType>::from_func(n, [&](auto i, auto j)
    {
        if constexpr (symm == MatrixSymmetry::Lower)
            if (i < j)
               return DType{};

        if constexpr (symm == MatrixSymmetry::Upper)
            if (i > j)
               return DType{};

        if constexpr (symm == MatrixSymmetry::Diagonal)
            if (i != j)
                return DType{};

        if constexpr (diag != Diag::NonUnit)
            if (i == j)
                if constexpr (diag == Diag::Unit)
                    return DType{ 1 };
                else if constexpr (diag == Diag::Skip)
                    return DType{ 0 };

        return unif(rng);
    });

    if constexpr (symm == MatrixSymmetry::Symmetric)
    {
       for (const auto i : A.iter_rows())
           for (const auto j : std::views::iota(i + 1U, A.cols()))
               A[i, j] = A[j, i] = (A[i, j] + A[j, i]) / 2.0;
    }

    std::vector<DType> b(n);
    std::generate(b.begin(), b.end(), [&]()
    {
        return unif(rng);
    });

    return std::make_pair(std::move(A), std::move(b));
}


template<std::floating_point DType>
void make_diag_dom(Matrix<DType>& M)
{
    const auto n = std::min(M.rows(), M.cols());
    for (std::size_t i{}; i < n; ++i)
    {
        const auto row = M.row(i);
        const auto row_sum = std::transform_reduce(
            row.cbegin(), row.cend(),
            DType{}, std::plus<DType>{}, [](const auto v) { return std::abs(v); }
        );

        const auto col = M.col(i);
        const auto col_sum = std::transform_reduce(
            col.cbegin(), col.cend(),
            DType{}, std::plus<DType>{}, [](const auto v) { return std::abs(v); }
        );

        M[i, i] = row_sum + col_sum;
    }

}


#endif //GENERATE_MATRIX_H
