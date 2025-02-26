#ifndef ARRAY_H
#define ARRAY_H

#include <cassert>
#include <cmath>
#include <concepts>
#include <numeric>
#include <ranges>
#include <vector>


template<std::floating_point scalar_t>
[[nodiscard]] constexpr scalar_t step(const scalar_t a, const scalar_t b, const int intervals) noexcept {
    if (intervals < 1) {
        return {0.0};
    }

    return (b - a) / static_cast<scalar_t>(intervals);
}


template<std::floating_point scalar_t>
[[nodiscard]] constexpr scalar_t step(const scalar_t len, const int intervals) noexcept {
    return step<scalar_t>(scalar_t{}, len, intervals);
}

/**
 * \brief Constructs a vector of equidistant #num points in the interval [start, end] inclusively
 *
 * @tparam scalar_t Floating point type, can be `float`, `double`, `long double`
 *
 * @param start left side of the interval, real
 * @param end right sie of the interval, real
 * @param points number of points in the interval
 *
 * @return vector of equidistant points in [start,end]
 */
template<std::floating_point scalar_t>
[[nodiscard]] constexpr std::vector<scalar_t> linspace(const scalar_t start, const scalar_t end, const int points) {
    assert(points >= 0);

    const auto dx = step(start, end, points - 1);
    auto rg = std::views::iota(0, points) | std::views::transform([&](auto i) { return start + i * dx; });

    return {rg.cbegin(), rg.cend()};
}


template<std::floating_point scalar_t>
[[nodiscard]] constexpr std::vector<scalar_t>
linspace(std::invocable<scalar_t> auto f, const scalar_t start, const scalar_t end, const int num) {
    assert(num >= 0);

    const auto dx = step(start, end, num - 1);
    auto rg = std::views::iota(0, num) | std::views::transform([&](auto i) { return f(start + i * dx); });

    return {rg.cbegin(), rg.cend()};
}


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
            [&one](const auto ti, const auto ri) -> value_type { return std::abs(ti / ri - one); });
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
