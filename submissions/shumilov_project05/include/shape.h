#ifndef SHAPE_H
#define SHAPE_H

#include <stdexcept>

#include <fmt/core.h>

#include "neighborhood.h"


struct Shape2D
{
    int m_rows{ 1 };
    int m_cols{ 1 };

    [[nodiscard]]
    constexpr auto rows() const
    {
        return m_rows;
    }

    [[nodiscard]]
    constexpr auto cols() const
    {
        return m_cols;
    }

    [[nodiscard]]
    constexpr Shape2D(const int rows_, const int cols_)
        : m_rows{ rows_ }
        , m_cols{ cols_ }
    {
        if (rows() < 1)
        {
            throw std::invalid_argument(
                fmt::format("`rows` must be positive: {}", rows())
            );
        }

        if (cols() < 1)
        {
            throw std::invalid_argument(
                fmt::format("`cols` must be positive: {}", cols())
            );
        }
    }

    [[nodiscard]]
    constexpr explicit Shape2D(const int n) : Shape2D{ n, n } {}

    [[nodiscard]]
    constexpr Shape2D() = default;

    [[nodiscard]]
    constexpr auto nelems() const
    {
        return rows() * cols();
    }

    [[nodiscard]]
    constexpr auto is_square() const
    {
        return rows() == cols();
    }

    [[nodiscard]]
    constexpr auto padded(const Padding& padding) const
    {
        return Shape2D{
            rows() + padding.north + padding.south,
            cols() + padding.north + padding.south,
        };
    }
};

#endif //SHAPE_H
