#ifndef LINALG_MATRIX_H
#define LINALG_MATRIX_H

#include <cstddef>  // for size_t
#include <iostream>
#include <cassert>

#include <vector>
#include <concepts>
#include <ranges>
#include <utility>
#include <numeric>

#include <fmt/format.h>
#include <fmt/ranges.h>


enum class MatrixSymmetry : char {
    Upper   = 'U',
    Lower   = 'L',
    Symmetric = 'S',
    Diagonal = 'D',
    // Hermitian = 'H',
    General = 'G',
};


enum class Diag : char {
    NonUnit = 'N',
    Unit = 'U'
};


[[nodiscard]]
constexpr auto flat_from_pair_idx(const std::size_t row, const std::size_t col, const std::size_t cols) -> std::size_t
{
    return row * cols + col;
}


[[nodiscard]]
constexpr auto pair_from_flat_idx(const std::size_t flat, const std::size_t cols) -> std::pair<std::size_t, std::size_t>
{
    return std::make_pair(flat / cols, flat % cols);
}


template<std::floating_point scalar_t>
class Matrix {
    using idx_t = std::size_t;

    class Shape {
        std::ptrdiff_t m_rows{};
        std::ptrdiff_t m_cols{};

    public:
        [[nodiscard]] constexpr
        Shape(const std::ptrdiff_t rows, const std::ptrdiff_t cols) : m_rows{rows}, m_cols{cols}
        {
            if (rows < 0 or cols < 0) {
                throw std::invalid_argument(
                    fmt::format("#rows and #cols must be non-negative integers: ({:d},{:d})", m_rows, m_cols)
                );
            }
        }

        [[nodiscard]] explicit constexpr
        Shape(const std::ptrdiff_t rows) : Shape{rows, rows} {}

        [[nodiscard]] constexpr
        auto square() const noexcept -> bool
        {
            return m_rows == m_cols;
        }

        [[nodiscard]] constexpr
        auto size() const noexcept -> std::ptrdiff_t
        {
            return m_rows * m_cols;
        }
    };

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
    // Default Constructors
    explicit Matrix() = default;
    Matrix(const Matrix& other) = default;
    Matrix(Matrix&& other) = default;

    // Constructor for initializing matrix with a constant value
    [[nodiscard]]
    constexpr Matrix(const idx_t rows, const idx_t cols, const scalar_t init_value)
        : m_rows{rows}
        , m_cols{cols}
        , m_data(rows * cols, init_value)
    {}

    // Move constructor by moving in vector of data
    [[nodiscard]]
    constexpr Matrix(const idx_t rows, const idx_t cols, std::vector<scalar_t>&& data) noexcept
        : m_rows{rows}
        , m_cols{cols}
        , m_data{std::move(data)}
    {}

    [[nodiscard]] static constexpr
    auto from_func(const idx_t rows, const idx_t cols, std::invocable<idx_t, idx_t> auto func) -> Matrix
    {
        auto r_rows = std::views::iota(idx_t{}, rows);
        auto r_cols = std::views::iota(idx_t{}, cols);
        auto r_data = std::views::cartesian_product(r_rows, r_cols)
                    | std::views::transform([&func](const auto& idx_pair) -> scalar_t {
                        return func(std::get<0>(idx_pair), std::get<1>(idx_pair));
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
        return Matrix{rows, cols, scalar_t{}};
    }

    [[nodiscard]] static constexpr
    auto zeros_like(const Matrix& other) -> Matrix
    {
        return Matrix::zeros(other.rows(), other.cols());
    }

    [[nodiscard]] static constexpr
    auto zeros(const idx_t rows) -> Matrix
    {
        return Matrix::zeros(rows, rows);
    }

    [[nodiscard]] static constexpr
    auto ones(const idx_t rows, const idx_t cols) -> Matrix
    {
        return Matrix{rows, cols, scalar_t{1}};
    }

    [[nodiscard]] static constexpr
    auto ones_like(const Matrix& other) -> Matrix
    {
        return Matrix::ones(other.rows(), other.cols());
    }

    [[nodiscard]] static constexpr
    auto ones(const idx_t rows) -> Matrix
    {
        return Matrix::ones(rows, rows);
    }

    [[nodiscard]] static constexpr
    auto eye(const idx_t rows, const idx_t cols) -> Matrix
    {
        return Matrix::from_func(rows, cols,
            [](const idx_t row, const idx_t col) -> scalar_t {
                return row == col ? scalar_t{1} : scalar_t{0};
            });
    }

    // Const accessor for matrix elements
    [[nodiscard]]
    constexpr auto operator()(const idx_t row, const idx_t col) -> scalar_t&
    {
        check_idx(row, col);
        return m_data[flat_idx(row, col)];
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
    constexpr auto data() const noexcept -> std::span<const scalar_t>
    {
        return m_data;
    }

    [[nodiscard]]
    constexpr auto size() const noexcept -> idx_t
    {
        return m_data.size();
    }


    [[nodiscard]]
    constexpr auto empty() const -> idx_t
    {
        return m_data.empty();
    }

    [[nodiscard]]
    constexpr auto is_square() const noexcept -> bool
    {
       return rows() == cols();
    }

    [[nodiscard]]
    constexpr auto row(const idx_t idx) const
    {
        return m_data
               | std::views::drop(idx * cols()) // find first element of idx'th row
               | std::views::take(cols())       // select the entire row
               | std::views::as_const;
    }

    [[nodiscard]]
    constexpr auto col(const idx_t idx) const
    {
        return m_data
               | std::views::drop(idx)  // Find first element of idx'th col
               | std::views::stride(cols())
               | std::views::as_const;
    }

    constexpr auto transpose() noexcept -> void
    {
        for (const auto r: std::views::iota(idx_t{}, rows())) {
            for (const auto c: std::views::iota(r + 1U, cols())) {
                std::swap(m_data[flat_idx(r, c)], m_data[flat_idx(c, r)]);
            }
        }
        std::swap(m_rows, m_cols);
    }

    constexpr auto norm() const noexcept -> scalar_t
    {
        return std::sqrt(
            std::transform_reduce(
                m_data.cbegin(), m_data.cend(), m_data.cbegin(),
                scalar_t{}, std::plus<scalar_t>{}, std::multiplies<scalar_t>{}
            )
        );
    }

    constexpr auto operator+=(const Matrix& rhs) noexcept -> Matrix&
    {
        assert(same_shape(rhs));
        for (const auto i : std::views::iota(0U, m_data.size())) {
            m_data[i] += rhs.m_data[i];
        }
        return *this;
    }

    constexpr auto operator-=(const Matrix& rhs) noexcept -> Matrix&
    {
        assert(same_shape(rhs));
        for (const auto i : std::views::iota(0U, m_data.size())) {
            m_data[i] -= rhs.m_data[i];
        }
        return *this;
    }

    [[nodiscard]]
    constexpr auto operator-() const -> Matrix
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

    [[nodiscard]]
    constexpr auto operator/=(const scalar_t value) -> Matrix
    {
        return *this *= scalar_t{1.0} / value;
    }

    void display(std::string_view name = "Matrix", std::string_view expr = "") const
    {
        std::cout << fmt::format("{}<{} x {}, {}> {}\n", name, m_rows, m_cols, typeid(scalar_t).name(), expr);
        std::cout << *this << "\n";
    }

    [[nodiscard]]
    auto to_string() const -> std::string
    {
        // Format each row
        auto lines =
            this->data()
            | std::views::as_const
            | std::views::chunk(this->cols())
            | std::views::transform(
                [](const auto& c) {
                    return fmt::format("[{: 12.6e}]", fmt::join(c, " "));
                }
            );

        return fmt::format("[{}]", fmt::join(lines, " \n "));
    }
};


template<std::floating_point T>
auto operator<<(std::ostream &out, const Matrix<T>& matrix) -> std::ostream & {
    out << matrix.to_string();
    return out;
}


template<std::floating_point scalar_t>
constexpr auto operator+(Matrix<scalar_t> lhs, const Matrix<scalar_t>& rhs) -> Matrix<scalar_t>
{
    assert(lhs.rows() == rhs.rows() && lhs.cols() == rhs.cols());
    lhs += rhs;
    return lhs;
}

template<std::floating_point scalar_t>
constexpr auto operator-(Matrix<scalar_t> lhs, const Matrix<scalar_t>& rhs) -> Matrix<scalar_t>
{
    assert(lhs.rows() == rhs.rows() && lhs.cols() == rhs.cols());
    lhs -= rhs;
    return lhs;
}


// Scalar Multiplication (copy)
template<std::floating_point U>
auto operator*(Matrix<U> scaled_matrix, const U scalar) -> Matrix<U>
{
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
    return matrix * (T{1} / scalar);
}


template<std::floating_point T>
auto operator/(const T scalar, const Matrix<T>& matrix) -> Matrix<T>
{
    return matrix / scalar;
}


template<std::floating_point T>
auto operator*(const Matrix<T>& lhs, const Matrix<T>& rhs) -> Matrix<T>
{
    assert(lhs.cols() == rhs.rows());

    return Matrix<T>::from_func(
        lhs.rows(), rhs.cols(),
        [&lhs, &rhs](const std::size_t i, const std::size_t j) -> T {
            auto row = lhs.row(i);
            auto col = rhs.col(j);
            return std::inner_product(row.cbegin(), row.cend(), col.cbegin(), T{});
        }
    );
}


template<std::floating_point T>
auto operator*(const Matrix<T>& M, std::span<const T> v) -> std::vector<T>
{
    assert(M.cols() == v.size());

    std::vector<T> result(v.size(), T{});
    for (const auto r : std::views::iota(0U, M.rows())) {
        result[r] = std::inner_product(v.cbegin(), v.cend(), M.row(r).cbegin(), T{});
    }

    return result;
}

#endif // LINALG_MATRIX_H
