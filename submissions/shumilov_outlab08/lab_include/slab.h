#ifndef SLAB_H
#define SLAB_H

#include <concepts>
#include <memory>

#include <fmt/core.h>
#include <nlohmann/json.hpp>

#include "methods/linalg/matrix.h"
#include "methods/utils/io.h"

#include "grid.h"


using json = nlohmann::json;

template<std::floating_point T>
class Flux;


template<std::floating_point T>
struct Slab
{
    Grid grid{};
    T total_cross_section{ 1 };
    T scattering_cross_section{ 1 };
    T source{ 1 };
    T slab_width{ 1 };

    constexpr Slab() noexcept = default;

    constexpr Slab(const Grid grid_, const T S_t, const T S_s, const T q, const T L)
        : grid{grid_}, total_cross_section{ S_t }, scattering_cross_section{ S_s }
        , source{ q }, slab_width{ L }
    {
        validate();
    }

    constexpr auto validate() const -> void
    {
        grid.validate();

        if (total_cross_section < 0)
        {
            throw std::invalid_argument(
                fmt::format("Total cross section must be non-negative: {: 14.6e}", total_cross_section)
            );
        }

        if (scattering_cross_section < 0)
        {
            throw std::invalid_argument(
                fmt::format("Scattering cross section must be non-negative: {: 14.6e}", scattering_cross_section)
            );
        }

        if (scattering_cross_section > total_cross_section)
        {
            throw std::invalid_argument(
                fmt::format(
                    "Total cross section must be larger than scattering cross section: {} < {}",
                    total_cross_section,
                    scattering_cross_section
                )
            );
        }

        if (source < 0)
        {
            throw std::invalid_argument(
                fmt::format("Fixed source must be non-negative: {: 14.6e}", source)
            );
        }

        if (slab_width <= 0)
        {
            throw std::invalid_argument(fmt::format("Slab width must be positive: {}", slab_width));
        }
    }


    [[nodiscard]]
    constexpr auto to_string(const int label_width = 40) const
    {
        return fmt::format(
            "Slab:\n"
            "{:s}\n"
            "Medium:\n"
            "\t{:{}s}: {: 14.6e}\n"
            "\t{:{}s}: {: 14.6e}\n"
            "\t{:{}s}: {: 14.6e}\n"
            "\t{:{}s}: {: 14.6e}",
            grid.to_string(),
            "Total Macroscopic Cross Section, St",
            label_width,
            total_cross_section,
            "Scattering Macroscopic Cross Section, Ss",
            label_width,
            scattering_cross_section,
            "Uniform Fixed Source Strength, q",
            label_width,
            source,
            "Slab Width, L",
            label_width,
            slab_width
        );
    }


    [[nodiscard]]
    auto init_scalar_flux() const -> std::vector<T>
    {
        return std::vector<T>(grid.num_spatial_cells, T{});
    }


    [[nodiscard]]
    auto init_point_angular_flux(const int angular_batch_size) const -> Matrix<T>
    {
        assert(0 < angular_batch_size && angular_batch_size <= grid.num_angular_points);

        const auto rows = static_cast<std::size_t>(angular_batch_size);
        const auto cols = static_cast<std::size_t>(grid.num_spatial_points());

        return Matrix<T>::zeros(rows, cols);
    }

    [[nodiscard]]
    auto init_point_angular_flux() const -> Matrix<T>
    {
        return init_point_angular_flux(grid.num_angular_points);
    }


    [[nodiscard]]
    auto init_cell_angular_flux(const int angular_batch_size) const -> Matrix<T>
    {
        assert(0 < angular_batch_size && angular_batch_size <= grid.num_angular_points);

        const auto rows = static_cast<std::size_t>(angular_batch_size);
        const auto cols = static_cast<std::size_t>(grid.num_spatial_cells);
        return Matrix<T>::zeros(rows, cols);
    }

    [[nodiscard]]
    auto init_cell_angular_flux() const -> Matrix<T>
    {
        return init_cell_angular_flux(grid.num_angular_points);
    }

    [[nodiscard]]
    auto init_flux(const int k_min, const int angular_batch_size) const -> Flux<T>
    {
        const auto [nodes, weights] = grid.angular_quadrature<T>(k_min, k_min + angular_batch_size - 1);

        #ifndef NDEBUG
        std::string result{};
        fmt::format_to(
            std::back_inserter(result), "{:^{}}\n",
            fmt::format("{:^5} {:^24} {:^24}", "i", "nodes", "weights"),
            80
        );

        for (std::size_t i{}; i < nodes.size(); ++i)
            fmt::format_to(
                std::back_inserter(result),
                "{:^{}}\n",
                fmt::format("{:<5d} {:> 24.16e} {:> 24.16e}", i + 1, nodes.at(i), weights.at(i)),
                80
            );

        fmt::print(
            std::cerr,
            "{2:^{1}}\n{0:-^{1}}\n{3}",
            "",
            80,
            fmt::format("Gauss Quadrature, I = {:d}", nodes.size()),
            result
        );
        #endif

        return Flux<T>{
            std::make_shared<Slab<T>>(*this),
            init_scalar_flux(),
            init_point_angular_flux(angular_batch_size),
            init_cell_angular_flux(angular_batch_size),
            std::move(nodes), std::move(weights)
        };
    }

    [[nodiscard]]
    auto init_flux() const -> Flux<T>
    {
       return init_flux(1, grid.num_angular_points);
    }

    [[nodiscard]]
    auto delta() const -> T
    {
        return slab_width / static_cast<T>(grid.num_spatial_cells);
    }


    [[nodiscard]]
    static auto from_file(std::istream& input) -> Slab
    {
        const Slab slab{
            Grid::from_file(input),
            read_nonnegative_value<T>(input, "Total Cross section"),
            read_nonnegative_value<T>(input, "Scattering cross section"),
            read_nonnegative_value<T>(input, "Fixed Source"),
            read_positive_value<T>(input, "Slab Width")
        };

        slab.validate();

        return slab;
    }

    NLOHMANN_DEFINE_TYPE_INTRUSIVE(Slab<T>, grid, total_cross_section, scattering_cross_section, source, slab_width)
};


#endif //SLAB_H
