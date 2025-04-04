#ifndef LAB08_GRID_H
#define LAB08_GRID_H

#include <concepts>

#include <fmt/core.h>
#include <nlohmann/json.hpp>


#include "methods/legendre.h"

using json = nlohmann::json;


struct Grid2D
{
    static constexpr int MIN_SPATIAL_CELLS{ 1 };
    static constexpr int MIN_ANGULAR{ 2 };

    int num_angular_points{ MIN_ANGULAR };
    int num_spatial_cells{ MIN_SPATIAL_CELLS };

    [[nodiscard]]
    constexpr Grid2D() = default;


    [[nodiscard]]
    constexpr Grid2D(const int N, const int I)
        : num_angular_points{ N }
      , num_spatial_cells{ I }
    {
        validate();
    }

    constexpr auto validate() const -> void
    {
        if (num_spatial_cells < MIN_SPATIAL_CELLS)
            throw std::invalid_argument(
                fmt::format("#spatial cells must be at least {:d}: {:d}", MIN_SPATIAL_CELLS, num_spatial_cells)
            );

        if (num_angular_points < MIN_ANGULAR)
            throw std::invalid_argument(
                fmt::format("#angular points must be at least {:d}: {:d}", MIN_ANGULAR, num_angular_points)
            );
    }


    template<std::floating_point T>
    [[nodiscard]] constexpr
    auto angular_quadrature(const FixedPointIterSettings<T>& settings = FixedPointIterSettings<T>{ 1e-12, 5 }) const
    {
        return gauss_legendre_quadrature<T>(num_angular_points, settings);
    }

    template<std::floating_point T>
    [[nodiscard]] constexpr
    auto angular_quadrature(
        const int k_min, const int k_max,
        const FixedPointIterSettings<T>& settings = FixedPointIterSettings<T>{ 1e-12, 5 }) const
    {
        return gauss_legendre_quadrature<T>(num_angular_points, k_min, k_max, settings);
    }


    template<std::floating_point T>
    constexpr auto spatial_points(const T length)
    {
        return linspace<T>(T{}, length, num_spatial_points());
    }


    [[nodiscard]]
    constexpr auto num_spatial_points() const noexcept
    {
        return num_spatial_cells + 1;
    }


    [[nodiscard]]
    constexpr auto size() const noexcept
    {
        return num_angular_points * num_spatial_cells;
    }


    [[nodiscard]]
    constexpr auto to_string(const int label_width = 40) const
    {
        return fmt::format(
            "Slab Grid:\n"
            "\t{:{}s}: {:d}\n"
            "\t{:{}s}: {:d}",
            "Number of angular grid points, N",
            label_width,
            num_angular_points,
            "Number of spatial cells, I",
            label_width,
            num_spatial_cells
        );
    }

    [[nodiscard]]
    static auto from_file(std::istream& input) -> Grid2D
    {
        const Grid2D grid{
            read_positive_value<int>(input, "Number of angular points") * 2,
            read_positive_value<int>(input, "Number of spatial points"),
        };

        return grid;
    }

    NLOHMANN_DEFINE_TYPE_INTRUSIVE_WITH_DEFAULT(Grid2D, num_spatial_cells, num_angular_points)
};
#endif // LAB08_GRID_H
