#ifndef ARRAY_SHAPE2D_H
#define ARRAY_SHAPE2D_H

#include <cstddef> // For std::ptrdiff_t
#include <stdexcept>
#include <utility> // For std::pair

#include <fmt/core.h>


class Shape2D
{
    using index_type = std::ptrdiff_t;
    using Idx2D = std::pair<index_type, index_type>;

    index_type m_rows{};
    index_type m_cols{};

    [[nodiscard]]
    constexpr auto _ravel(const index_type row, const index_type col) const -> index_type
    {
        return row * m_cols + col;
    }

    public:
        [[nodiscard]]
        constexpr Shape2D(const index_type rows, const index_type cols) noexcept
            : m_rows{ rows }
            , m_cols{ cols }
        {
            if (m_rows < 0)
            {
                throw std::invalid_argument(
                    fmt::format("`rows` must be non-negative: {}", m_rows)
                );
            }

            if (m_cols < 0)
            {
                throw std::invalid_argument(
                    fmt::format("`cols` must be non-negative: {}", m_cols)
                );
            }
        }

        [[nodiscard]]
        constexpr auto operator[](const Idx2D& idx) const noexcept -> index_type
        {
            const auto& [row, col] = idx;
            return _ravel(row, col);
        }

        [[nodiscard]]
        constexpr auto at(const Idx2D& idx) const -> index_type
        {
            const auto& [row, col] = idx;
            if (row < 0 or row >= rows())
                throw std::out_of_range(fmt::format("Row index is out of [0, {}) range: {}", cols(), row));

            if (col < 0 or col >= cols())
                throw std::out_of_range(fmt::format("Column index is out of [0, {}) range: {}", cols(), col));

            return _ravel(row, col);
        }

        [[nodiscard]]
        constexpr auto rows() const noexcept -> index_type
        {
            return m_rows;
        }

        [[nodiscard]]
        constexpr auto cols() const noexcept -> index_type
        {
            return m_cols;
        }

        [[nodiscard]]
        constexpr auto size() const noexcept -> index_type
        {
            return rows() * cols();
        }

        [[nodiscard]]
        constexpr auto empty() const noexcept -> bool
        {
           return not static_cast<bool>(size());
        }

        [[nodiscard]]
        constexpr auto operator==(const Shape2D& rhs) const noexcept -> bool
        {
            return rows() == rhs.rows() && cols() == rhs.cols();
        }
};



#endif //ARRAY_SHAPE2D_H
