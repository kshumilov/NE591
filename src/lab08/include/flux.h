#ifndef FLUX_H
#define FLUX_H

#include <concepts>
#include <memory>
#include <vector>

#include <fmt/core.h>
#include <nlohmann/json.hpp>

#include "methods/array.h"
#include "methods/linalg/matrix.h"

#include "slab.h"
#include "utils.h"


template<std::floating_point T>
struct Flux
{
    std::shared_ptr<Slab<T>> slab;
    std::vector<T> scalar_flux{};
    Matrix<T> point_angular_flux{};
    Matrix<T> cell_angular_flux{};
    std::vector<T> angular_nodes{};
    std::vector<T> angular_weights{};

    [[nodiscard]]
    constexpr auto to_string() const
    {
        auto result = fmt::format("{:^5s} {:^14s}\n", "i", "flux");
        for (int i{}; i < slab->grid.num_spatial_cells; ++i)
            fmt::format_to(std::back_inserter(result), "{:5d} {: 14.6e}\n", i + 1, scalar_flux.at(i));
        return result;
    }


    [[nodiscard]]
    constexpr auto Q(const int i, std::span<const T> cell_scalar_flux) const -> T
    {
        assert(0 <= i);
        const auto i_ = static_cast<std::size_t>(i);
        assert(i_ < cell_scalar_flux.size());
        return 0.5 * (cell_scalar_flux[i_] * slab->scattering_cross_section + slab->source);
    }

    auto forward_sweep(const std::size_t n, const T mu_n, std::span<const T> cell_scalar_flux) -> void
    {
        assert(mu_n >= 0);
        assert(cell_scalar_flux.size() == cell_angular_flux.cols());
        const auto two_mu_over_delta = 2.0 * mu_n / slab->delta();
        for (std::size_t i{}; i < cell_angular_flux.cols(); ++i)
        {
            const auto Qi = Q(i, cell_scalar_flux);
            cell_angular_flux.at(n, i) =
                (Qi + two_mu_over_delta * point_angular_flux.at(n, i)) / (
                    two_mu_over_delta + slab->total_cross_section);

            point_angular_flux.at(n, i + 1) =
                2 * cell_angular_flux.at(n, i) - point_angular_flux.at(n, i);
        }
    }

    auto backward_sweep(const std::size_t n, const T mu_n, std::span<const T> cell_scalar_flux) -> void
    {
        assert(mu_n <= 0);
        const auto two_mu_over_delta = -2.0 * mu_n / slab->delta();
        const int cols = static_cast<int>(cell_angular_flux.cols());
        for (int i = cols - 1; i > -1; --i)
        {
            const auto Qi = Q(i, cell_scalar_flux);
            cell_angular_flux.at(n, i) =
                (Qi + two_mu_over_delta * point_angular_flux.at(n, i + 1)) / (
                    two_mu_over_delta + slab->total_cross_section);

            point_angular_flux.at(n, i) =
                2 * cell_angular_flux.at(n, i) - point_angular_flux.at(n, i + 1);
        }
    }

    auto sweep(std::span<const T> cell_scalar_flux)
    {
        assert(cell_scalar_flux.size() == cell_angular_flux.cols());
        for (std::size_t n{}; n < cell_angular_flux.rows(); ++n)
        {
            const auto mu_n = angular_nodes.at(n);
            if (mu_n >= 0)
                forward_sweep(n, mu_n, cell_scalar_flux);
            else
                backward_sweep(n, mu_n, cell_scalar_flux);
        }
    }

    auto integrate_cell_angular_flux(std::span<T> cell_scalar_flux) -> void
    {
        assert(cell_scalar_flux.size() == cell_angular_flux.cols());

        // Update Scalar flux
        for (std::size_t i{}; i < cell_angular_flux.cols(); ++i)
        {
            cell_scalar_flux[i] = std::transform_reduce(
                angular_weights.cbegin(),
                angular_weights.cend(),
                cell_angular_flux.col(i).cbegin(),
                T{},
                std::plus<T>{},
                std::multiplies<T>{}
            );
        }
    }

    auto run_source_iteration(std::span<const T> curr, std::span<T> next) -> void
    {
        sweep(curr);
        integrate_cell_angular_flux(next);
    }

};
#endif //FLUX_H
