#ifndef LINALG_MATRIX_H
#define LINALG_MATRIX_H

// Types and IO
#include <cstddef>  // for size_t
#include <iostream>
#include <cassert>
#include <concepts>
#include <stdexcept>

// Data Structures and Algorithms
#include <vector>
#include <ranges>
#include <utility>

// 3rd-party Dependencies
#include <fmt/format.h>
#include <fmt/ranges.h>
#include <nlohmann/json.hpp>

// LinAlg operations on Vectors
#include "methods/linalg/vec.h"
#include "methods/linalg/blas.h"

using json = nlohmann::json;


template<class IndexType = std::size_t>
using Idx2D = std::pair<IndexType, IndexType>;


template<class IndexType = std::size_t>
[[nodiscard]] constexpr
auto ravel2d(const IndexType i, const IndexType j, const IndexType lda) noexcept -> IndexType
{
    return i * lda + j;
}


template<class IndexType = std::size_t>
[[nodiscard]] constexpr
auto unravel2d(const IndexType flat, const IndexType lda) -> Idx2D<IndexType>
{
    assert(lda > 0);
    return std::make_pair<IndexType, IndexType>(flat / lda, flat % lda);
}


template<std::floating_point scalar_t>
class Matrix {
public:
    using idx_t = std::size_t;

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
    constexpr Matrix(const idx_t rows, const idx_t cols, std::vector<scalar_t>&& data)
        : m_rows{rows}
        , m_cols{cols}
        , m_data{std::move(data)}
    {
        if (m_data.size() != rows * cols) {
            throw std::invalid_argument(fmt::format(
                "Matrix size must match data size: data[{}] != {}", m_data.size(), m_rows * m_cols
            ));
        }
    }

    [[nodiscard]] static constexpr
    auto from_func(const idx_t rows, const idx_t cols, std::invocable<idx_t, idx_t> auto func) -> Matrix
    {
        std::vector<scalar_t> data(rows * cols);
        for (const auto r : std::views::iota(idx_t{}, rows)) {
            for (const auto c : std::views::iota(idx_t{}, cols)) {
                data[ravel2d(r, c, cols)] = func(r, c);
            }
        }

        return Matrix{rows, cols, std::move(data)};
    }

    [[nodiscard]] static constexpr
    auto from_func(const idx_t rows, std::invocable<idx_t, idx_t> auto func) -> Matrix
    {
        return Matrix::from_func(rows, rows, func);
    }

    [[nodiscard]] static constexpr
    auto from_permutation(std::span<const idx_t> permutation) -> Matrix
    {
        auto P = Matrix::zeros(permutation.size());
        for (std::size_t i = 0; i < P.rows(); ++i) {
            P[i, permutation[i]] = scalar_t{1};
        }
        return P;
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
            [](const idx_t i, const idx_t j) -> scalar_t {
                return i == j ? scalar_t{1} : scalar_t{};
            });
    }

    [[nodiscard]]
    constexpr bool is_valid_idx(const idx_t row, const idx_t col) const noexcept
    {
        return (row <= rows()) || (col <= cols());
    }

    [[nodiscard]]
    constexpr auto ravel(const idx_t row, const idx_t col) const noexcept -> idx_t
    {
        return ravel2d<idx_t>(row, col, cols());
    }

    [[nodiscard]]
    constexpr auto unravel(const idx_t idx) const -> std::pair<idx_t, idx_t>
    {
        return unravel2d<idx_t>(idx, cols());
    }

    [[nodiscard]]
    constexpr auto same_shape(const Matrix& other) const noexcept -> bool
    {
        return rows() == other.rows() && cols() == other.cols();
    }

    // Accessor for matrix elements
    [[nodiscard]]
    constexpr auto operator[](const idx_t row, const idx_t col) -> scalar_t&
    {
        return m_data[ravel(row, col)];
    }

    [[nodiscard]]
    constexpr auto at(const idx_t row, const idx_t col) -> scalar_t&
    {
        if (row >= rows() || col >= cols()) {
            throw std::out_of_range(fmt::format(
                "Index pair ({}, {}) is out of range for {}", row, col, shape_info()
            ));
        }
        return m_data.at(ravel(row, col));
    }

    // Const accessor for matrix elements
    [[nodiscard]]
    constexpr auto operator[](const idx_t row, const idx_t col) const -> const scalar_t&
    {
        return m_data[ravel(row, col)];
    }

    [[nodiscard]]
    constexpr auto at(const idx_t row, const idx_t col) const -> const scalar_t&
    {
        return this->at(row, col);
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
    constexpr auto iter_rows() const noexcept
    {
        return std::views::iota(idx_t{}, rows());
    }

    [[nodiscard]]
    constexpr auto iter_cols() const noexcept
    {
        return std::views::iota(idx_t{}, cols());
    }

    [[nodiscard]]
    constexpr auto row(const idx_t idx) const
    {
        return data()
               | std::views::drop(idx * cols()) // find first element of idx'th row
               | std::views::take(cols());       // select the entire row
    }

    [[nodiscard]]
    constexpr auto col(const idx_t idx) const
    {
        return data()
               | std::views::drop(idx)  // Find first element of idx'th col
               | std::views::stride(cols());
    }

    constexpr auto transpose() noexcept -> void
    {
        for (const auto r: iter_rows()) {
            for (const auto c: std::views::iota(r + 1U, cols())) {
                std::swap(m_data[ravel(r, c)], m_data[ravel(c, r)]);
            }
        }
        std::swap(m_rows, m_cols);
    }

    [[nodiscard]]
    constexpr auto norm() const noexcept -> scalar_t
    {
        return norm_l2(m_data);
    }

    constexpr void swaprows(const idx_t r1, const idx_t r2) noexcept
    {
        for (const auto c : iter_cols()) {
            std::swap(m_data[ravel(r1, c)], m_data[ravel(r2, c)]);
        }
    }

    constexpr auto operator+=(const Matrix& rhs) -> Matrix&
    {
        m_data += rhs.m_data;
        return *this;
    }

    constexpr auto operator-=(const Matrix& rhs) -> Matrix&
    {
        m_data -= rhs.m_data;
        return *this;
    }

    [[nodiscard]]
    constexpr auto operator-() const noexcept -> Matrix
    {
        return *this * scalar_t{-1.0};
    }

    [[nodiscard]]
    constexpr auto operator*=(const scalar_t value) -> Matrix&
    {
        this->m_data *= value;
        return *this;
    }

    [[nodiscard]]
    constexpr auto operator/=(const scalar_t value) -> Matrix&
    {
        this->m_data /= value;
        return *this;
    }

    void display(std::string_view name = "Matrix", std::string_view expr = "") const
    {
        std::cout << fmt::format("{}{:s} {}\n", name, shape_info(), expr);
        std::cout << *this << "\n";
    }

    [[nodiscard]]
    constexpr std::string shape_info() const
    {
       return fmt::format("<{:d} x {:d}, {:s}>", rows(), cols(), typeid(scalar_t).name());
    }

    [[nodiscard]]
    auto to_string(const std::string_view sep=" ") const -> std::string
    {
        // Format each row
        const auto lines =
            this->data()
            | std::views::as_const
            | std::views::chunk(this->cols())
            | std::views::transform(
                [sep](const auto& c) {
                    return fmt::format("[{: 12.6e}]", fmt::join(c, sep));
                }
            );

        return fmt::format("[{}]", fmt::join(lines, " \n "));
    }

    NLOHMANN_DEFINE_TYPE_INTRUSIVE(Matrix<scalar_t>, m_rows, m_cols, m_data)

private:
    idx_t m_rows{};          // Number of rows
    idx_t m_cols{};          // Number of columns
    std::vector<scalar_t> m_data{}; // Data storage in row-major order
};


template<std::floating_point T>
auto operator<<(std::ostream &out, const Matrix<T>& matrix) -> std::ostream & {
    out << matrix.to_string();
    return out;
}


template<std::floating_point scalar_t>
constexpr auto operator+(Matrix<scalar_t> lhs, const Matrix<scalar_t>& rhs) -> Matrix<scalar_t>
{
    assert(lhs.same_shape(rhs));
    lhs += rhs;
    return lhs;
}

template<std::floating_point scalar_t>
constexpr auto operator-(Matrix<scalar_t> lhs, const Matrix<scalar_t>& rhs) -> Matrix<scalar_t>
{
    assert(lhs.same_shape(rhs));
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

    auto C = Matrix<T>::zeros(lhs.rows(), rhs.cols());
    gemm(lhs, rhs, C);
    return C;
}


template<std::floating_point T>
auto operator*(const Matrix<T>& lhs, std::span<const T> rhs) -> std::vector<T>
{
    assert(lhs.cols() == rhs.size());

    std::vector<T> result(rhs.size());
    gemv<T>(lhs, rhs, result);
    return result;
}

template<std::floating_point T>
auto operator*(const Matrix<T>& M, const std::vector<T>& v) -> std::vector<T>
{
   return operator*(M, std::span{v});
}

#endif // LINALG_MATRIX_H
