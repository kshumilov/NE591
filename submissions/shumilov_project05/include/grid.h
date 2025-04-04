//
// Created by Kirill Shumilov on 4/2/25.
//

#ifndef GRID_H
#define GRID_H

#include <fmt/format.h>

#include "io.h"
#include "neighborhood.h"
#include "shape.h"


struct Grid2D {
    Shape2D shape{};

    [[nodiscard]]
    constexpr Grid2D(const int nx, const int ny) : shape{nx,ny} {}

    [[nodiscard]]
    explicit constexpr Grid2D(const int n) : Grid2D{n, n} {}

    [[nodiscard]]
    explicit constexpr Grid2D() = default;

    [[nodiscard]]
    explicit constexpr Grid2D(const Shape2D& shape_) : shape{ shape_ } {}

    [[nodiscard]]
    constexpr auto points_x() const
    {
        return shape.rows();
    }

    [[nodiscard]]
    constexpr auto points_y() const
    {
        return shape.cols();
    }

    [[nodiscard]]
    constexpr auto points() const
    {
        return points_x() * points_y();
    }

    [[nodiscard]]
    constexpr auto cells_x() const
    {
        return points_x() - 1;
    }

    [[nodiscard]]
    constexpr auto cells_y() const
    {
        return points_y() - 1;
    }

    [[nodiscard]]
    constexpr auto cells() const
    {
        return cells_x() * cells_y();
    }

    [[nodiscard]]
    constexpr auto padded(const Padding& padding) const
    {
        return Grid2D{ shape.padded(padding) };
    }

    [[nodiscard]]
    static auto from_file(std::istream& input)
    {
        return Grid2D{
            read_positive_value<int>(input, "M"),
            read_positive_value<int>(input, "N"),
        };
    }
};

template<>
struct fmt::formatter<Grid2D>
{
    [[nodiscard]]
    constexpr auto parse(format_parse_context& ctx)
    {
        return ctx.begin();
    }

    auto format(const Grid2D& grid, fmt::format_context& ctx) const
    {
        return fmt::format_to(ctx.out(),
            "Non-Zero Grid Points, M x N: {:5d} x {:5d}",
            grid.points_x(), grid.points_y()
        );
    }
};
#endif //GRID_H
