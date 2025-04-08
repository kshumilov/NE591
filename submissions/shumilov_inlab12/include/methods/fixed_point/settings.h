#ifndef FIXED_POINT_SETTINGS_H
#define FIXED_POINT_SETTINGS_H

#include <concepts>

#include "fmt/format.h"

#include "utils/io.h"

template<std::floating_point ErrorType = long double>
struct FPSettings
{
    static constexpr ErrorType DEFAULT_TOLERANCE{1.0e-8};
    static constexpr int DEFAULT_MAX_ITER{100};

    ErrorType tolerance{DEFAULT_TOLERANCE};
    int max_iter{DEFAULT_MAX_ITER};

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


    enum class FPSettingParamOrder
    {
        ToleranceFirst, MaxIterFirst,
    };


    template<FPSettingParamOrder Order = FPSettingParamOrder::ToleranceFirst>
    [[nodiscard]]
    static auto from_file(std::istream& input)
    {
        if constexpr (Order == FPSettingParamOrder::ToleranceFirst)
        {
            return FPSettings{
                read_positive_value<ErrorType>(input, "tolerance"),
                read_positive_value<int>(input, "max_iter"),
            };
        }
        else
        {
            const auto max_iter_ = read_positive_value<int>(input, "max_iter");
            const auto tolerance_ = read_positive_value<ErrorType>(input, "tolerance");
            return FPSettings{
                tolerance_,
                max_iter_,
            };
        }
    }
};


template<std::floating_point ErrorType>
struct fmt::formatter<FPSettings<ErrorType>>
{
    [[nodiscard]]
    constexpr auto parse(format_parse_context& ctx)
    {
        return ctx.begin();
    }

    [[nodiscard]]
    auto format(const FPSettings<ErrorType>& fps, fmt::format_context& ctx) const
    {
        return fmt::format_to(ctx.out(),
            "Tolerance: {:g}\n"
            "Maximum #Iterations: {:L}",
            fps.tolerance,
            fps.max_iter
        );
    }
};


#endif