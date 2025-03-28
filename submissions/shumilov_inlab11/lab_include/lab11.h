#ifndef INPUTS_H
#define INPUTS_H

#include <concepts>
#include <memory>

#include <fmt/core.h>
#include <fmt/format.h>
#include <fmt/ostream.h>

#include "methods/linalg/Axb/pcg.h"

#include "lab/lab.h"

template<std::floating_point T>
struct Lab11
{
    FPSettings<T> iter_settings{};
    std::shared_ptr<LinearSystem<T>> system{};
    Info info{
        .title = "NE 501 Inlab #11",
        .author = "Kirill Shumilov",
        .date = "03/27/2025",
        .description = "Implementation of PCG solver with Jacobi Preconditioner for Ax=b systems"
    };

    [[nodiscard]]
    constexpr auto build_solver() const -> PCG<T>
    {
        return PCG<T>{ iter_settings, { 100, PreconditionerType::Jacobi } };
    }

    [[nodiscard]]
    constexpr auto run() const
    {
        const auto solver = build_solver();
        return solver.solve(system);
    }

    [[nodiscard]]
    static auto from_file(std::istream& input) -> Lab11<T>
    {
        auto settings = FPSettings<T>::from_file(input);
        return Lab11<T>{
            std::move(settings),
            std::make_shared<LinearSystem<T>>(
                std::move(LinearSystem<T>::from_file(input))
            )
        };
    }

    static void print_result(std::ostream& out, const std::pair<bool, std::unique_ptr<PCGState<T>>>& result)
    {
        fmt::println(out, "================================================================================");
        fmt::println(out, "{:^80s}", "Results");
        fmt::println(out, "--------------------------------------------------------------------------------");
        fmt::println(out, "PCG Converged: {}", result.first);
        fmt::println(out, "PCG Error: {}", result.second->error());
        fmt::println(out, "PCG Iterations: {}", result.second->iteration());
        fmt::println(out, "Solution Vector, x:");
        fmt::println(out, "[{: 14.8e}]", fmt::join(result.second->x, " "));
        fmt::println(out, "================================================================================");
    }
};


template<std::floating_point T>
struct fmt::formatter<Lab11<T>>
{

    [[nodiscard]]
    constexpr auto parse(format_parse_context& ctx)
    {
        return ctx.begin();
    }

    auto format(const Lab11<T>& lab, fmt::format_context& ctx) const
    {
        const auto solver = lab.build_solver();
        const auto M = make_preconditioner(solver.params.preconditioner_type, *lab.system);
        const auto precond_matrix = M->get_full_matrix();
        return fmt::format_to(ctx.out(),
            "{6}"
            "{0:^{1}}\n"
            "{5:-<{1}}\n"
            "{2}\n"
            "{3}\n\n"
            "Preconditioner Matrix, Minv: {4:F: 14.8e}",
            "Inputs", COLUMN_WIDTH,
            lab.iter_settings,
            *lab.system, precond_matrix, "", lab.info
        );
    }
};


#endif //INPUTS_H
