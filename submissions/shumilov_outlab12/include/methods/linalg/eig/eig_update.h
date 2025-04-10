#ifndef EIG_UPDATE_H
#define EIG_UPDATE_H

#include <fmt/format.h>

#include "utils/io.h"


enum class EigenValueUpdate: int
{
    PowerIteration = 0, RayleighQuotient = 1,
};


[[nodiscard]]
inline auto read_eig_update_algorithm(std::istream& in) -> EigenValueUpdate
{
    const auto algo = read_nonnegative_value<int>(in, "Eigenvalue Update Policy");
    if (algo > 3)
    {
        throw std::runtime_error(fmt::format("Invalid algorithm code, must be 0/1: {}", algo));
    }

    switch (algo)
    {
        case 0:
            return EigenValueUpdate::PowerIteration;
        case 1:
            return EigenValueUpdate::RayleighQuotient;
        default:
            throw std::runtime_error("Invalid algorithm code");
    }
}


template<>
struct fmt::formatter<EigenValueUpdate, char>
{
    constexpr auto parse(format_parse_context& ctx)
    {
        return ctx.begin();
    }

    constexpr auto format(const EigenValueUpdate val, format_context& ctx) const
    {
        switch (val)
        {
            case EigenValueUpdate::PowerIteration:
                return fmt::format_to(ctx.out(), "Power Iteration");
            case EigenValueUpdate::RayleighQuotient:
                return fmt::format_to(ctx.out(), "Rayleigh Quotient");
            default:
                std::unreachable();
        }
        return ctx.out();
    }
};



#endif //EIG_UPDATE_H
