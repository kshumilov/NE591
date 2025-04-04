//
// Created by Kirill Shumilov on 4/2/25.
//

#ifndef CONFIG_H
#define CONFIG_H

#include <concepts>

#include <fmt/format.h>

#include "io.h"


enum class Algorithm : int
{
    PointJacobi = 1, GaussSeidel = 2, SuccessiveOverRelaxation = 3,
};


template<>
struct fmt::formatter<Algorithm, char>
{
    constexpr auto parse(format_parse_context& ctx)
    {
        return ctx.begin();
    }

    constexpr auto format(const Algorithm val, format_context& ctx) const
    {
        switch (val)
        {
            // case Algorithm::LUP:
            //     return fmt::format_to(ctx.out(), "LU with Partial Row Pivoting");
            case Algorithm::PointJacobi:
                return fmt::format_to(ctx.out(), "Point Jacobi");
            case Algorithm::GaussSeidel:
                return fmt::format_to(ctx.out(), "Gauss-Seidel");
            case Algorithm::SuccessiveOverRelaxation:
                return fmt::format_to(ctx.out(), "Successive Over Relaxation");
            default:
                std::unreachable();
        }
        return ctx.out();
    }
};


[[nodiscard]]
inline auto read_algorithm(std::istream& in) -> Algorithm
{
    const auto algo = read_nonnegative_value<int>(in, "Algorithm");
    if (algo > 3)
    {
        throw std::runtime_error(fmt::format("Invalid algorithm code, must be 1/2/3: {}", algo));
    }

    switch (algo)
    {
        case 1:
            return Algorithm::PointJacobi;
        case 2:
            return Algorithm::GaussSeidel;
        case 3:
            return Algorithm::SuccessiveOverRelaxation;
        default:
            throw std::runtime_error("Invalid algorithm code");
    }
}


template<std::floating_point T = long double>
struct FixedPointSettings
{
    static constexpr T DEFAULT_TOLERANCE{1.0e-8};
    static constexpr int DEFAULT_MAX_ITER{100};

    int max_iter{DEFAULT_MAX_ITER};
    T tolerance{DEFAULT_TOLERANCE};

    [[nodiscard]]
    constexpr explicit FixedPointSettings
    (
        const int max_iter_ = DEFAULT_MAX_ITER,
        const T tolerance_ = DEFAULT_TOLERANCE
    )
        : max_iter(max_iter_)
        , tolerance(tolerance_)
    {
        if (max_iter <= int{})
            throw std::invalid_argument(fmt::format("`max_iter` must be positive: {: d}", max_iter));

        if (tolerance <= T{})
            throw std::invalid_argument(fmt::format("`tolerance` must be positive: {: 12.6e}", tolerance));
    }

    [[nodiscard]]
    constexpr auto operator==(const FixedPointSettings& other) const
    {
        return max_iter == other.tolerance and isclose(tolerance, other.tolerance);
    }

    [[nodiscard]]
    static auto from_file(std::istream& input)
    {
        return FixedPointSettings{
            read_positive_value<int>(input, "max_iter"),
            read_positive_value<T>(input, "tolerance"),
        };
    }
};


template<std::floating_point T>
struct fmt::formatter<FixedPointSettings<T>>
{
    [[nodiscard]]
    constexpr auto parse(format_parse_context& ctx)
    {
        return ctx.begin();
    }

    auto format(const FixedPointSettings<T>& fps, fmt::format_context& ctx) const
    {
        return fmt::format_to(ctx.out(),
            "Tolerance: {:g}\n"
            "Maximum #Iterations: {:L}",
            fps.tolerance,
            fps.max_iter
        );
    }
};


template<std::floating_point T>
struct SolverConfig {
    Algorithm algorithm{};
    FixedPointSettings<T> settings{};
    T relaxation_factor{1.0};

    [[nodiscard]]
    static auto from_file(std::istream& input)
    {
        const auto algo = read_algorithm(input);
        return SolverConfig{
            algo,
            FixedPointSettings<T>::from_file(input),
            algo == Algorithm::SuccessiveOverRelaxation
                ? read_positive_value<T>(input, "relaxation_factor")
                : 1.0,
        };
    }
};


template<std::floating_point T>
struct fmt::formatter<SolverConfig<T>>
{
    [[nodiscard]]
    constexpr auto parse(format_parse_context& ctx)
    {
        return ctx.begin();
    }

    auto format(const SolverConfig<T>& config, fmt::format_context& ctx) const
    {
        return fmt::format_to(ctx.out(),
            "Algorithm: {}\n"
            "{}{}",
            config.algorithm,
            config.settings,
            config.algorithm == Algorithm::SuccessiveOverRelaxation
                ? fmt::format("\nRelaxation Factor: {:g}", config.relaxation_factor) : ""
        );
    }
};

#endif //CONFIG_H

