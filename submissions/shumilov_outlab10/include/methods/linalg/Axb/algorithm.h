#ifndef LINALG_AXB_ALGORITHM_H
#define LINALG_AXB_ALGORITHM_H

#include <utility>

#include <fmt/format.h>


enum class AxbAlgorithm: int
{
    LUP = 0,
    PointJacobi = 1,
    GaussSeidel = 2,
    SuccessiveOverRelaxation = 3,
    ConjugateGradient = 4,
};


template<>
struct fmt::formatter<AxbAlgorithm, char>
{
    constexpr auto parse(format_parse_context& ctx)
    {
        return ctx.begin();
    }

    constexpr auto format(const AxbAlgorithm val, format_context& ctx) const
    {
        switch (val)
        {
            case AxbAlgorithm::LUP:
                return fmt::format_to(ctx.out(), "LU with Partial Row Pivoting");
            case AxbAlgorithm::PointJacobi:
                return fmt::format_to(ctx.out(), "Point Jacobi");
            case AxbAlgorithm::GaussSeidel:
                return fmt::format_to(ctx.out(), "Gauss-Seidel");
            case AxbAlgorithm::SuccessiveOverRelaxation:
                return fmt::format_to(ctx.out(), "Successive Over Relaxation");
            case AxbAlgorithm::ConjugateGradient:
                return fmt::format_to(ctx.out(), "Conjugate Gradients");
            default:
                std::unreachable();
        }
        return ctx.out();
    }
};

#endif
