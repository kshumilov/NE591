#ifndef LINALG_AXB_ALGORITHM_H
#define LINALG_AXB_ALGORITHM_H

#include <utility>

#include <fmt/format.h>

enum class AxbAlgorithm
{
    LUP,
    PointJacobi,
    GaussSeidel,
    SuccessiveOverRelaxation,
};

template<>
struct fmt::formatter<AxbAlgorithm, char>
{
    template<class ParseContext>
    constexpr ParseContext::iterator parse(ParseContext& ctx)
    {
        return ctx.begin();
    }


    template<class FmtContext>
    constexpr auto format(const AxbAlgorithm val, FmtContext& ctx) const
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
            default:
                std::unreachable();
        }
        return ctx.out();
    }
};

#endif
