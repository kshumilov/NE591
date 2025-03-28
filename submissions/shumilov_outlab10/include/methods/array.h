#ifndef ARRAY_H
#define ARRAY_H

#include <cmath>
#include <concepts>
#include <numeric>
#include <ranges>

#include "methods/math.h"


[[nodiscard]] constexpr auto max_abs(const std::ranges::range auto &r) {
    return std::ranges::max(r | std::views::transform([](const auto &v) { return std::abs(v); }));
}

template<std::ranges::range U, std::ranges::range V = U>
[[nodiscard]] constexpr auto max_rel_diff(const U &test, const V &reference) {
    using value_type = std::common_type_t<std::ranges::range_value_t<U>, std::ranges::range_value_t<V>>;
    const auto zero = value_type{0};
    const auto one = value_type{1};
    return std::transform_reduce(
            test.cbegin(),
            test.cend(),
            reference.cbegin(),
            zero,
            [](const auto xi, const auto xj) constexpr -> value_type { return std::max(xi, xj); },
            [&](const auto ti, const auto ri) -> value_type
            {
                if (ri == zero and isclose(ti, zero)) return zero;
                return std::abs(ti / ri - one);
            });
}

template<std::ranges::range U, std::ranges::range V = U>
[[nodiscard]]
constexpr auto max_rel_err(const U& error, const V& value)
{
    using value_type = std::common_type_t<std::ranges::range_value_t<U>, std::ranges::range_value_t<V>>;
    const auto zero = value_type{0};
    return std::transform_reduce(
        error.cbegin(), error.cend(),
        value.cbegin(),
        zero,
        [](const auto xi, const auto xj) -> value_type { return std::max(xi, xj); },
        [](const auto ei, const auto vi) -> value_type { return rel_err(ei, vi); }
    );
}

template<std::ranges::range U, std::ranges::range V = U>
[[nodiscard]] constexpr auto max_abs_diff(const U &lhs, const V &rhs) {
    using value_type = std::common_type_t<std::ranges::range_value_t<U>, std::ranges::range_value_t<V>>;
    const auto zero = value_type{0};
    return std::transform_reduce(
            lhs.cbegin(),
            lhs.cend(),
            rhs.cbegin(),
            zero,
            [](const auto xi, const auto xj) -> value_type { return std::max(xi, xj); },
            [](const auto ui, const auto vi) -> value_type { return std::abs(ui - vi); });
}

#endif // ARRAY_H
