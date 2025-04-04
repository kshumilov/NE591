#ifndef RESULT_H
#define RESULT_H

#include <concepts>
#include <limits>

#include "block.h"


template<std::floating_point T>
struct FixedPointResult
{
    bool converged{};
    T error{ std::numeric_limits<T>::infinity() };
    int iterations{ 0 };
    T max_abs_residual{std::numeric_limits<T>::infinity()};
    Distributed2DBlock<T> x{};
};


template<std::floating_point T>
struct fmt::formatter<FixedPointResult<T>>
{
    [[nodiscard]]
    constexpr auto parse(format_parse_context& ctx)
    {
        return ctx.begin();
    }

    auto format(const FixedPointResult<T>& result, fmt::format_context& ctx) const
    {
        const auto& [converged, error, iterations, residual, x] = result;
        return fmt::format_to(ctx.out(),
            "Converged: {}\n"
            "#Iterations: {}\n"
            "Iterative Error: {}\n"
            "Max Abs Residual: {}\n"
            "Solution:\n{}",
            converged, iterations, error, residual,
            x.size() < 64
                ? x.padded_array_view().to_string()
                : fmt::format("<{}, {}>", x.info.padded_rows(), x.info.padded_cols())
        );
    }
};
#endif //RESULT_H
