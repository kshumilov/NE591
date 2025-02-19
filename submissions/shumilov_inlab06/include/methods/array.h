#ifndef ARRAY_H
#define ARRAY_H

#include <vector>
#include <concepts>
#include <ranges>
#include <cassert>
#include <cmath>


template<std::floating_point scalar_t>
[[nodiscard]] constexpr
scalar_t step(const scalar_t a, const scalar_t b, const int intervals) noexcept
{
    if (intervals < 1) {
        return {0.0};
    }

    return (b - a) / static_cast<scalar_t>(intervals);
}


template<std::floating_point scalar_t>
[[nodiscard]] constexpr
scalar_t step(const scalar_t len, const int intervals) noexcept
{
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
[[nodiscard]] constexpr
std::vector<scalar_t> linspace(const scalar_t start, const scalar_t end, const int points) {
    assert(points >= 0);

    const auto dx = step(start, end, points - 1);
    auto rg = std::views::iota(0, points)
            | std::views::transform([&](auto i) {
                return start + i * dx;
              });

    return { rg.cbegin(), rg.cend() };
}


template<std::floating_point scalar_t>
[[nodiscard]] constexpr
std::vector<scalar_t> linspace(std::invocable<scalar_t> auto f, const scalar_t start, const scalar_t end, const int num) {
    assert(num >= 0);

    const auto dx = step(start, end, num - 1);
    auto rg = std::views::iota(0, num)
            | std::views::transform([&](auto i) {
                return f(start + i * dx);
              });

    return { rg.cbegin(), rg.cend() };
}


[[nodiscard]] constexpr
auto max_abs(const std::ranges::range auto& container)
{
    return std::ranges::max(
        container | std::views::transform([](const auto& v) { return std::abs(v); })
    );
}

template<std::ranges::range U, std::ranges::range V = U>
[[nodiscard]] constexpr
auto max_abs_diff(const U& lhs, const V& rhs) {
    return std::transform_reduce(
        lhs.cbegin()
      , lhs.cend()
      , rhs.cbegin()
      , std::ranges::range_value_t<decltype(lhs)>{}
      , [](const auto xi, const auto xj) { return std::max(xi, xj); }
      , [](const auto xi, const auto xj) { return std::abs(xi - xj); }
    );
}

#endif //ARRAY_H