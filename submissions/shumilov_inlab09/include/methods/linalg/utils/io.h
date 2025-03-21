#ifndef LINALG_UTILS_H
#define LINALG_UTILS_H

#include <cstddef>
#include <concepts>
#include <functional>
#include <stdexcept>
#include <string_view>

#include <fmt/core.h>

#include "methods/utils/io.h"

#include "methods/linalg/matrix.h"

using namespace std::literals;

inline auto read_rank(std::istream& in, const std::string_view dim_name = ""sv) -> std::ptrdiff_t
{
    return read_positive_value<std::ptrdiff_t>(in, dim_name);
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


template<std::floating_point T, MatrixSymmetry symmetry = MatrixSymmetry::General>
auto read_square_matrix(std::istream& in, const std::size_t rank) -> Matrix<T>
{
    return read_matrix<T, symmetry>(in, rank, rank);
}


template<std::floating_point scalar_t>
auto read_embedded_matrix(
    std::istream& in,
    const std::size_t total_rows, const std::size_t total_cols,
    const std::size_t r0, const std::size_t c0,
    const std::size_t rows, const std::size_t cols
) -> Matrix<scalar_t>
{
    assert(r0 < total_rows);
    assert(r0 + rows < total_rows);
    assert(c0 < total_cols);
    assert(c0 + cols < total_cols);
    auto select = [&](const size_t i, const size_t j) -> bool
    {
        return r0 <= i && i < r0 + rows && c0 <= j && j < c0 + cols;
    };

    return read_matrix_elements<scalar_t>(in, total_rows, total_cols, select);
}


template<class T>
auto print(const Matrix<T>& matrix, std::string_view header = "Matrix"sv, std::ostream& out = std::cerr) -> void {
    fmt::println(out, "{:s}:", header);
    out << matrix << "\n\n";
}

#endif // LINALG_UTILS_H
