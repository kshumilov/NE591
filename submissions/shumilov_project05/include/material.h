//
// Created by Kirill Shumilov on 4/2/25.
//

#ifndef MATERIAL_H
#define MATERIAL_H

#include <concepts>

#include <fmt/format.h>

#include "io.h"

template<std::floating_point T>
struct MaterialProperties {
    T diffusion_coeff{};  // D
    T absorption_xs{};    // Sigma_a

    [[nodiscard]]
    static auto from_file(std::istream& input)
    {
       return MaterialProperties{
           read_positive_value<T>(input, "D"),
           read_nonnegative_value<T>(input, "Sa"),
       };
    }
};


template<std::floating_point T>
struct fmt::formatter<MaterialProperties<T>>
{
    [[nodiscard]]
    constexpr auto parse(format_parse_context& ctx)
    {
        return ctx.begin();
    }

    auto format(const MaterialProperties<T>& material, fmt::format_context& ctx) const
    {
        return fmt::format_to(ctx.out(),
            "Diffusion Coefficient, D: {:14.8e}\n"
            "Macroscopic Removal Cross Section, Sa: {:14.8e}",
            material.diffusion_coeff, material.absorption_xs
        );
    }
};
#endif //MATERIAL_H
