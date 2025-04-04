#ifndef REGION_H
#define REGION_H

#include <concepts>

#include <fmt/format.h>

#include "io.h"

template<std::floating_point T>
struct Region
{
    T lx{};  // Region length in x
    T ly{};  // Region length in y

    [[nodiscard]]
    constexpr auto hx(const int cells_x) const { return lx / cells_x; }

    [[nodiscard]]
    constexpr auto hy(const int cells_y) const { return ly / cells_y; }

    [[nodiscard]]
    static auto from_file(std::istream& input)
    {
       return Region{
           read_positive_value<T>(input, "a"),
           read_positive_value<T>(input, "b"),
       };
    }
};


template<std::floating_point T>
struct fmt::formatter<Region<T>>
{
    [[nodiscard]]
    constexpr auto parse(format_parse_context& ctx)
    {
        return ctx.begin();
    }

    auto format(const Region<T>& region, fmt::format_context& ctx) const
    {
        return fmt::format_to(ctx.out(),
            "Space Dimensions, a x b: {:14.8e} x {:14.8e}",
            region.lx, region.ly
        );
    }
};
#endif //REGION_H
