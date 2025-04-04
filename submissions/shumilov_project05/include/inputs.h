#ifndef INPUTS_H
#define INPUTS_H

#include <concepts>
#include <vector>

#include <fmt/format.h>
#include <fmt/ranges.h>

#include "config.h"
#include "grid.h"
#include "material.h"
#include "region.h"
#include "stencil.h"
#include "matrix.h"

#include "io.h"


template<std::floating_point T>
struct Inputs
{
    SolverConfig<T> solver_config{};
    Region<T> region{};
    Grid2D inner_grid{};
    MaterialProperties<T> material{};
    std::vector<T> source{};

    [[nodiscard]]
    constexpr auto build_stencil() const
    {
        const auto outer_grid = inner_grid.padded(Padding{1});
        const auto hx = region.hx(outer_grid.cells_x());
        const auto hy = region.hy(outer_grid.cells_y());
        const auto D = material.diffusion_coeff;
        const auto Sa = material.absorption_xs;

        const auto horizontal = -D / (hx * hx);
        const auto vertical = -D / (hy * hy);
        const auto center = -T{ 2.0 } * (horizontal + vertical) + Sa;

        return Stencil2D<T>{
            center,
            Neighborhood2D<T>{
                vertical,
                vertical,
                horizontal,
                horizontal,
            },
        };
    }

    [[nodiscard]]
    static auto from_file(std::istream& input)
    {
        const auto config = SolverConfig<T>::from_file(input);
        const auto region = Region<T>::from_file(input);
        const auto grid = Grid2D::from_file(input);
        const auto material = MaterialProperties<T>::from_file(input);
        return Inputs{
            config, region, grid, material,
            read_vector<T>(input, grid.points()),
        };
    }
};


template<std::floating_point T>
struct fmt::formatter<Inputs<T>>
{
    [[nodiscard]]
    constexpr auto parse(format_parse_context& ctx)
    {
        return ctx.begin();
    }

    auto format(const Inputs<T>& inputs, fmt::format_context& ctx) const
    {
        return fmt::format_to(ctx.out(),
            "{:^{}s}\n"
            "{:-^{}s}\n"
            "{}\n"
            "{:.^{}s}\n"
            "{}\n"
            "{}\n"
            "{}\n"
            "{:.^{}s}\n"
            "Source:\n{}\n"
            "{:=^{}s}\n",
            "Inputs", 80,
            "", 80,
            inputs.solver_config,
            "", 80,
            inputs.region,
            inputs.inner_grid,
            inputs.material,
            "", 80,
            inputs.source.size() < 64
                ? MatrixView<const T>(inputs.source, inputs.inner_grid.shape).to_string()
                : fmt::format("<{}, {}>", inputs.inner_grid.points_x(), inputs.inner_grid.points_y()),
            "", 80
        );
    }
};
#endif //INPUTS_H
