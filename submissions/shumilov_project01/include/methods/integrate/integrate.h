#ifndef INTEGRATE_H
#define INTEGRATE_H

#include <concepts>
#include <utility>

#include "methods/array.h"

#include "quadrature.h"
#include "trapezoidal.h"
#include "simpson.h"
#include "gauss.h"


/**
 * @brief Integrate f over interval [a.b] using Trapezoidal, Simpson, or Gauss-Legendre Quadrature
 *
 * @tparam scalar_t type of scalar used, can be `float`, `double`, or `long double`
 *
 * @param f Function to integrate
 * @param a lower bound of integration
 * @param b upper bound of integration
 * @param points number of points sampled, n >= 2
 * @param quad Quadrature type
 *
 * @return Integral approximation of a function f over interval [a,b]
 */
template <std::floating_point scalar_t>
auto integrate(
    std::invocable<scalar_t> auto& f,
    const scalar_t a, const scalar_t b, const int points,
    const Quadrature quad
) -> scalar_t
{
    switch (quad) {
        case Quadrature::Trapezoidal: {
            const auto y = linspace<scalar_t>(f, a, b, points);
            return trapezoidal<scalar_t>(y, step<scalar_t>(a, b, points - 1));
        }
        case Quadrature::Simpson: {
            const auto y = linspace<scalar_t>(f, a, b, points);
            return simpson<scalar_t>(y, step<scalar_t>(a, b, points - 1));
        }
        case Quadrature::Gauss: {
            return gauss<scalar_t>(f, a, b, points);
        }
        default: {
            std::unreachable();
        }
    }
}

#endif //INTEGRATE_H