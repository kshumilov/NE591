#ifndef GENERATE_MATRIX_H
#define GENERATE_MATRIX_H

#include <cstddef>
#include <concepts>
#include <utility>
#include <vector>
#include <algorithm>

#include "methods/linalg/matrix.h"

template<std::floating_point DType>
[[nodiscard]]
auto build_system(const std::size_t n) -> std::pair<Matrix<DType>, std::vector<DType>> {
    std::vector<DType> b(n);
    std::ranges::fill(b, DType{1});

    auto A = Matrix<DType>::from_func(n, [&](auto i, auto j) {
        if (i != j) {
            return DType{-1} / (static_cast<DType>(i) + static_cast<DType>(j));
        }
        return DType{};
    });

    for (std::size_t i{}; i < n; ++i) {
        A[i, i] = DType{1} / static_cast<DType>(n)
                - std::ranges::fold_left(A.row(i), DType{}, std::plus<DType>{});
    }

    return std::make_pair(std::move(A), std::move(b));
}

#endif //GENERATE_MATRIX_H
