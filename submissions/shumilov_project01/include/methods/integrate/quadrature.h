#ifndef INTEGRATE_QUADRATURE_H
#define INTEGRATE_QUADRATURE_H

#include <utility>

#include <fmt/format.h>

enum class Quadrature {
    Trapezoidal,
    Simpson,
    Gauss,
};


template<>
struct fmt::formatter<Quadrature, char> {
    template<class ParseContext>
    constexpr ParseContext::iterator parse(ParseContext& ctx)
    {
        return ctx.begin();
    }

    template<class FmtContext>
    constexpr auto format(const Quadrature q, FmtContext& ctx) const
    {
        switch (q) {
            case Quadrature::Trapezoidal:
                return fmt::format_to(ctx.out(), "Trapezoidal");
            case Quadrature::Simpson:
                return fmt::format_to(ctx.out(), "Simpson");
            case Quadrature::Gauss:
                return fmt::format_to(ctx.out(), "Gauss-Legendre");
            default:
                std::unreachable();
        }
        return ctx.out();
    }
};


#endif // INTEGRATE_QUADRATURE_H
