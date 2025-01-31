#ifndef GAUSS_H
#define GAUSS_H

#include <cmath>
#include <concepts>
#include <functional>
#include <numeric>

#include "legendre.h"


/**
 * @brief Calculate Quadrature weights for Legendre Polynomial of degree l
 *
 * @tparam scalar_t type of scalar used, can be `float`, `double`, or `long double`
 *
 * @param l Degree of legendre polynomial
 * @param x Quadrature node
 *
 * @return Quadrature weight for x'th node
 */
template<std::floating_point scalar_t>
constexpr auto gauss_weight(const std::size_t l, const scalar_t x) -> scalar_t
{
    assert(l >= 2);
    const scalar_t num {2.0 * (1.0 - x * x)};
    const scalar_t denom_sqrt {static_cast<scalar_t>(l + 1U) * legendre(x, l + 1U)};
    return num / (denom_sqrt * denom_sqrt);
}


/**
 * \brief Integrates the real valued 1D function over the range [a, b] using Gauss-Legendre Quadrature
 *
 * @tparam scalar_t type of scalar used, can be `float`, `double`, or `long double`
 *
 * @param f Function to integrate
 * @param a lower bound of integration
 * @param b upper bound of integration
 * @param n number of points sampled, n >= 2
 *
 * @return Value of the integral
 */
template<std::floating_point scalar_t>
constexpr auto gauss(
    std::function<scalar_t(scalar_t)> f,
    const scalar_t a, const scalar_t b,
    const std::size_t n = 2U) -> scalar_t
{
    const auto nodes = legendre_roots<scalar_t>(n);

    // Scale nodes from [-1, 1] to [a, b]
    auto scale = [&](const scalar_t x) constexpr -> scalar_t {
        return 0.5 * (a * (1.0 - x) + b * (1.0 + x));
    };

    return std::transform_reduce(
        nodes.cbegin(), nodes.cend(),
        0.0, std::plus<scalar_t>{},
        [&](const auto x) {
            return gauss_weight(n, x) * f(scale(x));
        }
    );
}

#endif //GAUSS_H
