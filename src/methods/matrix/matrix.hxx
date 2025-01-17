#ifndef MATRIX_H
#define MATRIX_H

#include <cstddef>  // for size_t
#include <cassert>
#include <functional>
#include <concepts>

#include <vector>
#include <ranges>
#include <numeric>

#include <fmt/core.h>
#include <fmt/ranges.h>


template<std::floating_point T>
class Matrix {
    using idx_t = std::size_t;

    idx_t m_rows{}; // Number of rows
    idx_t m_cols{}; // Number of columns
    std::vector<T> m_data{}; // Data storage in row-major order

    void check_idx(const idx_t row, const idx_t col) const {
        assert(row < m_rows);
        assert(col < m_cols);
    }

    [[nodiscard]]
    auto flat_idx(const idx_t row, const idx_t col) const -> idx_t {
        return row * m_cols + col;
    }

    [[nodiscard]]
    auto pair_idx(const idx_t idx) const -> std::pair<idx_t, idx_t> {
        return {idx / m_cols, idx % m_cols};
    }

    [[nodiscard]]
    auto same_shape(const Matrix& other) const -> bool {
        return m_rows == other.m_rows && m_cols == other.m_cols;
    }

public:
    // Constructor for initializing matrix with a constant value
    Matrix(const size_t rows, const size_t cols, const T init_value = T{})
        : m_rows{rows}
        , m_cols{cols}
        , m_data(rows * cols, init_value)
    {}

    // Constructor for initializing matrix with a function
    Matrix(const idx_t rows, const idx_t cols, std::function<T(idx_t, idx_t)> func)
        : m_rows{rows}
        , m_cols{cols}
//        , m_data {
//              std::ranges::iota_view(0U, size())
//              | std::views::transform([&](const idx_t flat_idx) -> T {
//                  auto [row, col] = pair_idx(flat_idx);
//                  return func(row, col);
//                })
//              | std::ranges::to<std::vector>()
//          }
    {
	auto r = std::ranges::iota_view(0U, size())
                 | std::views::transform([&](const idx_t flat_idx) -> T {
                  auto [row, col] = pair_idx(flat_idx);
                  return func(row, col);
                 });
	m_data.reserve(m_rows * m_cols);
	std::ranges::copy(r, std::back_inserter(m_data));
    }

    // Const accessor for matrix elements
    auto operator()(const idx_t row, const idx_t col) const -> const T& {
        check_idx(row, col);
        return m_data[flat_idx(row, col)];
    }

    // Get number of rows
    [[nodiscard]]
    constexpr auto rows() const -> std::size_t {
        return m_rows;
    }

    // Get number of columns
    [[nodiscard]]
    constexpr auto cols() const -> std::size_t {
        return m_cols;
    }

    [[nodiscard]]
    constexpr auto size() const -> std::size_t {
        return m_rows * m_cols;
    }

    auto row_view(const idx_t idx) const {
        return
            m_data
            | std::views::drop(idx * m_cols)  // find first element of idx'th row
            | std::views::take(m_cols)  // select the entire row
            | std::views::as_const;
    }

    auto col_view(const size_t idx) const {
        return
            m_data
            | std::views::drop(idx)
            | std::views::stride(m_rows)
            | std::views::as_const;
    }

    auto operator+=(const Matrix& rhs) -> Matrix& {
        assert(same_shape(rhs));
        for (std::tuple<T&, T> elem : std::views::zip(m_data, rhs.m_data)) {
            std::get<0>(elem) += std::get<1>(elem);
        }
        return *this;
    }

    auto operator-=(const Matrix& rhs) -> Matrix& {
        assert(same_shape(rhs));
        for (std::tuple<T&, T> elem : std::views::zip(m_data, rhs.m_data)) {
            std::get<0>(elem) -= std::get<1>(elem);
        }
        return *this;
    }

    auto operator+(const Matrix& rhs) -> Matrix {
        assert(same_shape(rhs));
        Matrix<T> new_matrix = { *this };
        new_matrix += rhs;
        return new_matrix;
    }

    auto operator-(const Matrix& rhs) -> Matrix {
        assert(same_shape(rhs));
        Matrix new_matrix = { *this };
        new_matrix -= rhs;
        return new_matrix;
    }

    auto operator-()const -> Matrix {
        return *this * T{-1.0};
    }

    auto operator*=(const T value) -> Matrix {
        for (auto& elem : m_data) {
            elem *= value;
        }
        return *this;
    }

    auto operator/=(const T value) -> Matrix {
        return *this *= T{1.0} / value;
    }

    void display(std::string_view name, std::string_view expr = "") const {
        std::cout << fmt::format("{}<{} x {}, {}> {}\n", name, m_rows, m_cols, typeid(T).name(), expr);
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
auto operator*(const Matrix<U>& matrix, const U scalar) -> Matrix<U> {
    auto scaled_matrix { matrix };
    scaled_matrix *= scalar;
    return scaled_matrix;
}

template<std::floating_point U>
auto operator*(const U scalar, const Matrix<U>& matrix) -> Matrix<U> {
    return matrix * scalar;
}

template<std::floating_point T>
auto operator/(const Matrix<T>& matrix, const T scalar) -> Matrix<T> {
    return matrix * (1 / scalar);
}

template<std::floating_point T>
auto operator/(const T scalar, const Matrix<T>& matrix) -> Matrix<T> {
    return matrix / scalar;
}

template<std::floating_point T>
auto operator*(const Matrix<T>& left, const Matrix<T>& right) -> Matrix<T> {
    assert(left.cols() == right.rows());
    return Matrix<T>{
        left.rows(), right.cols(),
        [&](const std::size_t i, const std::size_t j) -> T {
            auto row = left.row_view(i);
            auto col = right.col_view(j);
            T val { std::inner_product(row.cbegin(), row.cend(), col.cbegin(), T{}) };
            return val;
        }
    };
}

#endif //MATRIX_H
