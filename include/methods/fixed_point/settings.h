#ifndef FIXED_POINT_SETTINGS_H
#define FIXED_POINT_SETTINGS_H

#include <concepts>

#include "fmt/format.h"

template<std::floating_point ErrorType = long double>
struct FPSettings
{
    static constexpr ErrorType DEFAULT_TOLERANCE{1.0e-8};
    static constexpr int DEFAULT_MAX_ITER{100};

    ErrorType tolerance{};
    int max_iter{};

    [[nodiscard]]
    constexpr explicit FPSettings(const ErrorType tolerance_ = DEFAULT_TOLERANCE, const int max_iter_ = DEFAULT_MAX_ITER)
        : tolerance(tolerance_)
      , max_iter(max_iter_)
    {
        if (max_iter <= int{})
        {
            throw std::invalid_argument(fmt::format("`max_iter` must be positive: {: d}", max_iter));
        }

        if (tolerance <= ErrorType{})
        {
            throw std::invalid_argument(fmt::format("`tolerance` must be positive: {: 12.6e}", tolerance));
        }
    }

    [[nodiscard]]
    constexpr auto operator==(const FPSettings& other) const
    {
        return max_iter == other.tolerance and isclose(tolerance, other.tolerance);
    }

};


template<std::floating_point T>
struct fmt::formatter<FPSettings<T>>
{
    enum class Style
    {
        Repr, Pretty,
        PrettyWithHeader
    };

    Style style = Style::Repr;

    template<class ParseContext>
    constexpr auto parse(ParseContext& ctx)
    {
        auto it = ctx.begin();
        if (it == ctx.end() or *it == '}')
            return it;

        this->style = [&] {
            switch (*it++)
            {
                case 'r':
                    return Style::Repr;
                case 'p':
                    return Style::Pretty;
                case 'P':
                    return Style::PrettyWithHeader;
                default:
                    throw std::format_error("Invalid fixed point style");
            }
        }();

        return it;
    }

    template<class FormatContext>
    auto format(const FPSettings<T>& settings, FormatContext& ctx) const
    {
        if (style == Style::Repr)
        {
            return fmt::format_to(
                ctx.out(),
                "FPSettings(max_iter={:L}, tolerance={:g})",
                settings.max_iter,
                settings.tolerance
            );
        }

        return fmt::format_to(
            ctx.out(),
            "{2}"
            "{1}{3:.<{0}s}: {4:d}\n"
            "{1}{5:.<{0}s}: {6:.6g}",
            40,
            style == Style::PrettyWithHeader ? "\t" : "",
            style == Style::PrettyWithHeader ? "Fixed-Point Iteration:\n" : "",
            "Maximum Number of Iterations",
            settings.max_iter,
            "Tolerance",
            settings.tolerance
        );

    }
};


#endif