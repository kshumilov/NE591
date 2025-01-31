#ifndef ARRAY_H
#define ARRAY_H

#include <vector>
#include <concepts>
#include <ranges>
#include <cassert>
#include <functional>


template<std::floating_point scalar_t>
[[nodiscard]]
constexpr auto subdivide(const scalar_t a, const scalar_t b, const int intervals) noexcept -> scalar_t
{
    if (intervals < 1) {
        return {0.0};
    }

    if (intervals == 1) {
        return b - a;
    }
    return (b - a) / static_cast<scalar_t>(intervals);
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
[[nodiscard]]
auto linspace(const scalar_t start, const scalar_t end, const int points) -> std::vector<scalar_t> {
    assert(points >= 0);

    const auto dx = subdivide(start, end, points - 1);
    auto rg = std::views::iota(0, points)
            | std::views::transform([&](auto i) {
                return start + i * dx;
              });

    return { rg.cbegin(), rg.cend() };
}


template<std::floating_point scalar_t>
auto linspace(std::function<scalar_t(scalar_t)> f, const scalar_t start, const scalar_t end, const int num) -> std::vector<scalar_t> {
    assert(num >= 0);

    const auto dx = subdivide(start, end, num - 1);
    auto rg = std::views::iota(0, num)
            | std::views::transform([&](auto i) {
                return f(start + i * dx);
              });

    return { rg.cbegin(), rg.cend() };
}

#endif //ARRAY_H