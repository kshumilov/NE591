#ifndef SIMPSON_H
#define SIMPSON_H

#include <concepts>
#include <ranges>
#include <span>
#include <cassert>


/**
 * @brief Approximate integral of a function using Simpson quadrature
 *
 * @tparam scalar_t Floating point type, can be `float`, `double`, `long double`
 *
 * @param y Vector of f(xi) values, where nodes, xi, are equidistant
 * @param step Distance between adjacent points nodes x_{i+1} - x_{i]
 *
 * @return Integral Approximation
 */
template <std::floating_point scalar_t>
constexpr auto simpson(std::span<const scalar_t> y, const scalar_t step = scalar_t{1.0}) -> scalar_t
{
    if (y.size() == 0)
        return {0.0};

    assert((y.size() - 1U) % 2 == 0);

    scalar_t result {y.front() + y.back()};
    for (const auto i : std::views::iota(1U, y.size() - 1U)) {
        result += y[i] * (i % 2 ? 4 : 2);
    }
    return step / 3.0 * result;
}

#endif //SIMPSON_H