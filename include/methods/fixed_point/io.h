#ifndef FIXED_POINT_IO_H
#define FIXED_POINT_IO_H

#include "settings.h"

enum class FPSettingParamOrder
{
    ToleranceFirst, MaxIterFirst,
};


template<std::floating_point ErrorType = long double, FPSettingParamOrder Order = FPSettingParamOrder::ToleranceFirst>
[[nodiscard]]
auto settings_from_file(std::istream& input)
{
    if constexpr (Order == FPSettingParamOrder::ToleranceFirst)
    {
        return FPSettings<ErrorType>{
            read_positive_value<ErrorType>(input, "tolerance"),
            read_positive_value<int>(input, "max_iter"),
        };
    }
    else
    {
        const auto max_iter = read_positive_value<int>(input, "max_iter");
        const auto tolerance = read_positive_value<ErrorType>(input, "tolerance");
        return FPSettings<ErrorType>{
            tolerance,
            max_iter,
        };
    }
}

#endif // FIXED_POINT_IO_H
