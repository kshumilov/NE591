#ifndef MATRIX_H
#define MATRIX_H

#include <cstddef>  // for size_t
#include <iostream>
#include <cassert>
#include <functional>
#include <concepts>

#include <vector>
#include <concepts>
#include <ranges>
#include <numeric>

#include <fmt/format.h>
#include <fmt/ranges.h>


[[nodiscard]]
constexpr auto flat_from_pair_idx(const std::size_t row, const std::size_t col, const std::size_t cols) -> std::size_t
{
    return row * cols + col;
}

[[nodiscard]]
constexpr auto pair_from_flat_idx(const std::size_t flat, const std::size_t cols) -> std::pair<std::size_t, std::size_t>
{
    return {flat / cols, flat % cols};
}


template<std::floating_point scalar_t>
class Matrix {
    using idx_t = std::size_t;

    idx_t m_rows{};          // Number of rows
    idx_t m_cols{};          // Number of columns
    std::vector<scalar_t> m_data{}; // Data storage in row-major order

    void check_idx(const idx_t row, const idx_t col) const
    {
        assert(row < m_rows);
        assert(col < m_cols);
    }

    [[nodiscard]]
    constexpr auto flat_idx(const idx_t row, const idx_t col) const -> idx_t
    {
        return flat_from_pair_idx(row, col, m_cols);
    }

    [[nodiscard]]
    constexpr auto pair_idx(const idx_t idx) const -> std::pair<idx_t, idx_t>
    {
        return pair_from_flat_idx(idx, m_cols);
    }

    [[nodiscard]]
    constexpr auto same_shape(const Matrix& other) const -> bool
    {
        return m_rows == other.m_rows && m_cols == other.m_cols;
    }

public:
    // Constructor for initializing matrix with a constant value
    [[nodiscard]]
    constexpr Matrix(const idx_t rows, const idx_t cols, const scalar_t init_value)
        : m_rows{rows}
        , m_cols{cols}
        , m_data(rows * cols, init_value)
    {}

    // Move constructor by moving in vector of data
    constexpr Matrix(const idx_t rows, const idx_t cols, std::vector<scalar_t>&& data) noexcept
        : m_rows{rows}, m_cols{cols}
        , m_data{std::move(data)}
    {}


    [[nodiscard]] static constexpr
    auto from_func(const idx_t rows, const idx_t cols, std::invocable<idx_t, idx_t> auto func) -> Matrix
    {
        auto r_rows = std::views::iota(0U, rows);
        auto r_cols = std::views::iota(0U, cols);
        auto r_data = std::views::cartesian_product(r_rows, r_cols)
                    | std::views::transform([&func](const auto& idx_pair) -> scalar_t {
                        return func(idx_pair.first, idx_pair.second);
                      })
                    | std::views::common;

        std::vector<scalar_t> data{r_data.cbegin(), r_data.cend()};
        return Matrix{rows, cols, std::move(data)};
    }

    [[nodiscard]] static constexpr
    auto from_func(const idx_t rows, std::invocable<idx_t, idx_t> auto func) -> Matrix
    {
        return Matrix::from_func(rows, rows, func);
    }

    [[nodiscard]] static constexpr
    auto zeros(const idx_t rows, const idx_t cols) -> Matrix
    {
        return Matrix{rows, cols, scalar_t{0}};
    }

    [[nodiscard]] static constexpr
    auto zeros(const idx_t rows) -> Matrix
    {
        return Matrix::zeros(rows, rows);
    }

    [[nodiscard]] static constexpr
    auto ones(const idx_t rows, const idx_t cols) -> Matrix
    {
        return Matrix{rows, cols, scalar_t(1)};
    }

    [[nodiscard]] static constexpr
    auto ones(const idx_t rows) -> Matrix
    {
        return Matrix::ones(rows, rows);
    }

    [[nodiscard]] static constexpr
    auto eye(const idx_t rows) -> Matrix
    {
        return Matrix::from_func(rows,
            [](const idx_t row, const idx_t col) -> scalar_t {
                return row == col ? scalar_t{1} : scalar_t{0};
            });
    }

    // Const accessor for matrix elements
    [[nodiscard]]
    constexpr auto operator()(const idx_t row, const idx_t col) const -> const scalar_t&
    {
        check_idx(row, col);
        return m_data[flat_idx(row, col)];
    }

    // Get number of rows
    [[nodiscard]]
    constexpr auto rows() const noexcept -> idx_t
    {
        return m_rows;
    }

    // Get number of columns
    [[nodiscard]]
    constexpr auto cols() const noexcept -> idx_t
    {
        return m_cols;
    }

    [[nodiscard]]
    constexpr auto size() const noexcept -> idx_t
    {
        return m_rows * m_cols;
    }

    [[nodiscard]]
    constexpr auto is_square() const noexcept -> bool
    {
       return rows() == cols();
    }

    constexpr auto row_view(const idx_t idx) const
    {
        return m_data
               | std::views::drop(idx * m_cols) // find first element of idx'th row
               | std::views::take(m_cols)       // select the entire row
               | std::views::as_const;
    }

    constexpr auto col_view(const idx_t idx) const
    {
        return m_data
               | std::views::drop(idx)  // Find first element of idx'th col
               | std::views::stride(m_cols)
               | std::views::as_const;
    }

    constexpr auto transpose() -> void
    {
        for (const auto r: std::views::iota(0U, rows())) {
            for (const auto c: std::views::iota(r + 1U, cols())) {
                std::swap(m_data[flat_idx(r, c)], m_data[flat_idx(c, r)]);
            }
        }
        std::swap(m_rows, m_rows);
    }

    auto operator+=(const Matrix& rhs) -> Matrix&
    {
        assert(same_shape(rhs));
        for (std::tuple<scalar_t&, scalar_t> elem: std::views::zip(m_data, rhs.m_data)) {
            std::get<0>(elem) += std::get<1>(elem);
        }
        return *this;
    }

    auto operator-=(const Matrix& rhs) -> Matrix&
    {
        assert(same_shape(rhs));
        for (std::tuple<scalar_t&, scalar_t> elem: std::views::zip(m_data, rhs.m_data)) {
            std::get<0>(elem) -= std::get<1>(elem);
        }
        return *this;
    }

    auto operator+(const Matrix& rhs) -> Matrix
    {
        assert(same_shape(rhs));
        Matrix new_matrix = {*this};
        new_matrix += rhs;
        return new_matrix;
    }

    auto operator-(const Matrix& rhs) -> Matrix
    {
        assert(same_shape(rhs));
        Matrix new_matrix = {*this};
        new_matrix -= rhs;
        return new_matrix;
    }

    auto operator-() const -> Matrix
    {
        return *this * scalar_t{-1.0};
    }

    [[nodiscard]]
    constexpr auto operator*=(const scalar_t value) -> Matrix
    {
        for (auto& elem: m_data) {
            elem *= value;
        }
        return *this;
    }

    auto operator/=(const scalar_t value) -> Matrix
    {
        return *this *= scalar_t{1.0} / value;
    }

    void display(std::string_view name = "Matrix", std::string_view expr = "") const
    {
        std::cout << fmt::format("{}<{} x {}, {}> {}\n", name, m_rows, m_cols, typeid(scalar_t).name(), expr);
        std::cout << *this << "\n";
    }
};

template<std::floating_point T>
auto operator<<(std::ostream &out, const Matrix<T>& matrix) -> std::ostream & {
    auto fmt_row = [&](const size_t row_idx) -> std::string {
        return fmt::format("[{: 6.2f}]", fmt::join(matrix.row_view(row_idx), " "));
    };

    std::vector<std::string> lines {};

    lines.push_back(fmt::format("[{} ", fmt_row(0)));

    for (std::size_t i = 1; i < matrix.rows() - 1; ++i) {
        lines.push_back(fmt::format(" {} ", fmt_row(i)));
    }

    lines.push_back(fmt::format(" {}]", fmt_row(matrix.rows() - 1)));

    out << fmt::format("{}", fmt::join(lines, "\n"));
    return out;
}


// Scalar Multiplication (copy)
template<std::floating_point U>
auto operator*(const Matrix<U>& matrix, const U scalar) -> Matrix<U>
{
    auto scaled_matrix{matrix};
    scaled_matrix *= scalar;
    return scaled_matrix;
}

template<std::floating_point U>
auto operator*(const U scalar, const Matrix<U>& matrix) -> Matrix<U>
{
    return matrix * scalar;
}

template<std::floating_point T>
auto operator/(const Matrix<T>& matrix, const T scalar) -> Matrix<T>
{
    return matrix * (1 / scalar);
}

template<std::floating_point T>
auto operator/(const T scalar, const Matrix<T>& matrix) -> Matrix<T>
{
    return matrix / scalar;
}

template<std::floating_point T>
auto operator*(const Matrix<T>& left, const Matrix<T>& right) -> Matrix<T>
{
    assert(left.cols() == right.rows());

    return Matrix<T>::from_func(
        left.rows(), right.cols(),
        [&](const std::size_t i, const std::size_t j) -> T {
            auto row = left.row_view(i);
            auto col = right.col_view(j);
            T val{std::inner_product(row.cbegin(), row.cend(), col.cbegin(), T{})};
            return val;
        }
    );
}

template<std::floating_point T>
auto operator*(const Matrix<T>& mat, const std::vector<T>& vec) -> std::vector<T>
{
    assert(mat.cols() == vec.size());

    std::vector<T> result(vec.size(), 0.0);
    for (const auto r : std::views::iota(0U, mat.rows())) {
        result[r] = std::inner_product(vec.cbegin(), vec.cend(), mat.row_view(r).cbegin(), T{});
    }

    return result;
}

#endif //MATRIX_H
