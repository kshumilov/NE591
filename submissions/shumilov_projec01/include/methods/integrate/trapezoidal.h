#ifndef TRAPEZOIDAL_H
#define TRAPEZOIDAL_H


#include <concepts>
#include <span>
#include <numeric>

/**
 * @brief Approximate integral of a function using trapezoid quadrature
 *
 * @tparam scalar_t Floating point type, can be `float`, `double`, `long double`
 *
 * @param y Vector of f(xi) values, where nodes, xi, are equidistant
 * @param step Distance between adjacent points nodes x_{i+1} - x_{i]
 *
 * @return Integral Approximation
 */
template <std::floating_point scalar_t>
auto trapezoidal(std::span<const scalar_t> y, const scalar_t step = scalar_t{1.0}) -> scalar_t
{
    if (y.size() == 0U) {
        return {0.0};
    }

    return step * (
        (y.back() + y.front()) / 2.0 +
        std::reduce(
            y.cbegin() + 1, y.cend() - 1,
            scalar_t{0.}, std::plus<scalar_t>{}
        )
    );
}

#endif //TRAPEZOIDAL_H