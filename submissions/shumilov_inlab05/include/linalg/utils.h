#ifndef LINALG_UTILS_H
#define LINALG_UTILS_H

#include <istream>
#include <cstddef>
#include <concepts>
#include <functional>
#include <stdexcept>

#include "utils/io.h"
#include "linalg/matrix.h"


inline auto read_rank(std::istream& in) -> std::ptrdiff_t
{
    if (const auto result = read_value<std::ptrdiff_t>(in); result.has_value() and result.value() > 0) {
        return result.value();
    }

    throw std::runtime_error("Could not read `rank`.\n"
                             "Must be an integer larger than 1.");
}


template<std::floating_point scalar_t>
auto read_matrix_elements(std::istream& in, const std::size_t rows, const std::size_t cols, std::invocable<std::size_t, std::size_t> auto select)
{
    auto func = [&in, &select](const std::size_t row, const std::size_t col) -> scalar_t {
        if (select(row, col)) {
            if (const auto result = read_value<scalar_t>(in); result.has_value()) {
                return result.value();
            }

            throw std::runtime_error(
                std::format("Could not read matrix element ({:d}, {:d})\n"
                            "Must be a real value.", row + 1U, col + 1U)
            );
        }
        return scalar_t{};
    };

    return Matrix<scalar_t>::from_func(rows, cols, func);
}


template<std::floating_point scalar_t, MatrixSymmetry symmetry = MatrixSymmetry::General>
auto read_matrix(std::istream& in, const std::size_t rows, const std::size_t cols) -> Matrix<scalar_t>
{
    std::function<bool(std::size_t, std::size_t)> func;

    if constexpr (symmetry == MatrixSymmetry::Diagonal) {
        func = std::equal_to<std::size_t>{};
    }
    else if constexpr (symmetry == MatrixSymmetry::Lower) {
        func = std::greater_equal<std::size_t>{};
    }
    else if constexpr (symmetry == MatrixSymmetry::Upper) {
        func = std::less_equal<std::size_t>{};
    }
    else {
        func = [](const auto, const auto) constexpr -> bool {
            return true;
        };
    }

    return read_matrix_elements<scalar_t>(in, rows, cols, func);
}
#endif // LINALG_UTILS_H
