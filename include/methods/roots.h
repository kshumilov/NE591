#ifndef ROOTS_H
#define ROOTS_H

#include <concepts>
#include <cmath>
#include <utility>  // pair

#include "methods/optimize.h"


/**
 * @brief Newton-Raphson method for finding roots of a function.
 *
 * @tparam T The type of the input and output, typically `double`.
 *
 * @param f The function whose root is to be found.
 * @param df The derivative of the function `f`.
 * @param x0 The initial guess for the root.
 *
 * @param tol The tolerance for convergence.
 * @param max_iter The maximum number of iterations.
 *
 * @return A pair containing the estimated root and a boolean indicating convergence.
 */
template<std::floating_point T>
constexpr auto newton_raphson(
    std::invocable<T> auto& f, std::invocable<T> auto& df, const T x0,
    const T tol, const int max_iter
) -> std::pair<T, bool>
{
    auto g = [&](const auto x) constexpr {
        return x - f(x) / df(x);
    };

    auto converged = [tol](const auto x_next, const auto x) constexpr {
        return std::abs(x_next - x) < tol;
    };
    return fixed_point_iteration<T>(g, x0, converged, max_iter);
}

#endif //ROOTS_H
