#ifndef INPUTS_H
#define INPUTS_H

#include <concepts>
#include <memory>

#include <fmt/core.h>
#include <fmt/format.h>

#include "methods/linalg/Axb/conjugate_gradient.h"
#include "methods/linalg/Axb/utils.h"

#include "lab/lab.h"

template<std::floating_point T>
struct Lab10
{
    FPSettings<T> iter_settings{};
    std::shared_ptr<LinearSystem<T>> system{};
    Info info{
        .title = "NE 501 Outlab #09",
        .author = "Kirill Shumilov",
        .date = "03/21/2025",
        .description = "Preparation for implementation of CG solver for Ax=b systems"
    };

    [[nodiscard]]
    constexpr auto run() const
    {
        const CG<T> cg{ iter_settings };
        return cg.solve(system);
    }

    [[nodiscard]]
    static auto from_file(std::istream& input) -> Lab10<T>
    {
        auto settings = FPSettings<T>::from_file(input);
        return Lab10<T>{
            std::move(settings),
            std::make_shared<LinearSystem<T>>(
                std::move(LinearSystem<T>::from_file(input))
            )
        };
    }

    static void print_result(std::ostream& out, const std::pair<bool, std::unique_ptr<CGState<T>>>& result)
    {
        fmt::println(out, "================================================================================");
        fmt::println(out, "{:^80s}", "Results");
        fmt::println(out, "--------------------------------------------------------------------------------");
        fmt::println(out, "CG Converged: {}", result.first);
        fmt::println(out, "CG Error: {}", result.second->error());
        fmt::println(out, "CG Iterations: {}", result.second->iteration());
        fmt::println(out, "Solution Vector, x:");
        fmt::println(out, "[{: 14.8e}]", fmt::join(result.second->x, " "));
        fmt::println(out, "================================================================================");
    }
};


template<std::floating_point T>
struct fmt::formatter<Lab10<T>>
{

    [[nodiscard]]
    constexpr auto parse(format_parse_context& ctx)
    {
        return ctx.begin();
    }

    auto format(const Lab10<T>& lab, fmt::format_context& ctx) const
    {
        return fmt::format_to(ctx.out(),
            "{5}"
            "{0:^{1}}\n"
            "{4:-<{1}}\n"
            "{2}\n"
            "{3}",
            "Inputs", COLUMN_WIDTH,
            lab.iter_settings,
            *lab.system, "", lab.info
        );
    }
};


#endif //INPUTS_H
