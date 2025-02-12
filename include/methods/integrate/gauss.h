#ifndef INTEGRATE_GAUSS_H
#define INTEGRATE_GAUSS_H

#include <concepts>
#include <cassert>
#include <numeric>   // transform_reduce

#include "methods/legendre.h"


/**
 * @brief Calculate Quadrature weights for Legendre Polynomial of degree l
 *
 * @tparam scalar_t type of scalar used, can be `float`, `double`, or `long double`
 *
 * @param l Degree of legendre polynomial
 * @param x k'th root for Legendre Polynomial of degree l
 *
 * @return Quadrature weight for x'th node
 */
template<std::floating_point scalar_t>
constexpr auto gauss_weight(const int l, const scalar_t x) -> scalar_t
{
    assert(l >= 2);
    const scalar_t num {2.0 * (1.0 - x * x)};
    const scalar_t denom_sqrt {static_cast<scalar_t>(l + 1) * legendre(x, l + 1)};
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
    std::invocable<scalar_t> auto& f,
    const scalar_t a, const scalar_t b,
    const int n = 2
) -> scalar_t
{
    const auto nodes = legendre_roots<scalar_t>(n);

    // Scale nodes from [-1, 1] to [a, b]
    auto scale = [a, b](const scalar_t x) constexpr -> scalar_t {
        return 0.5 * (a * (1.0 - x) + b * (1.0 + x));
    };

    return (b - a) / 2.0 * std::transform_reduce(
        nodes.cbegin(), nodes.cend(),
        scalar_t{}, std::plus<scalar_t>{},
        [&](const auto x) {
            return gauss_weight(n, x) * f(scale(x));
        }
    );
}

#endif //INTEGRATE_GAUSS_H
