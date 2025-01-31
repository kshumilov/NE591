#ifndef INTEGRATE_H
#define INTEGRATE_H


#include <concepts>
#include <stdexcept>

#include "trapezoidal.h"
#include "simpson.h"
#include "gauss.h"
#include "array.h"


enum class Quadrature {
    Trapezoidal,
    Simpson,
    Gauss,
};


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
    std::function<scalar_t(scalar_t)> f,
    const scalar_t a, const scalar_t b, const int points,
    Quadrature quad
) -> scalar_t
{
    switch (quad) {
        case Quadrature::Trapezoidal: {
            const auto y = linspace<scalar_t>(f, a, b, points);
            return trapezoidal<scalar_t>(y, subdivide<scalar_t>(a, b, points - 1));
        }
        case Quadrature::Simpson: {
            const auto y = linspace<scalar_t>(f, a, b, points);
            return simpson<scalar_t>(y, subdivide<scalar_t>(a, b, points - 1));
        }
        case Quadrature::Gauss: {
            return gauss<scalar_t>(f, a, b, points);
        }
        default: {
            throw std::runtime_error("Invalid quadrature type");
        }
    }
}

#endif //INTEGRATE_H