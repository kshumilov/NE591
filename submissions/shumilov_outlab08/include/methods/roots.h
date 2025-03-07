#ifndef ROOTS_H
#define ROOTS_H

#include <concepts>
#include <cmath>
#include <utility>  // pair

#include "methods/optimize.h"


/**
 * @brief Newton-Raphson method for finding roots of a function.
 *
 * @tparam Dtype The type of the input and output, typically `double`.
 *
 * @param f The function whose root is to be found.
 * @param df The derivative of the function `f`.
 * @param x0 The initial guess for the root.
 * @param settings Fixed Point Iteration Settings
 *
 * @return A pair containing the estimated root and a boolean indicating convergence.
 */
template<std::floating_point Dtype>
constexpr auto newton_raphson(
    std::invocable<Dtype> auto f, std::invocable<Dtype> auto df, const Dtype x0,
    const FixedPointIterSettings<Dtype>& settings
) -> FixedPointIterResult<Dtype, Dtype>
{
    auto g = [&](const auto x) constexpr {
        return x - f(x) / df(x);
    };

    auto error = [&f](const auto x_next) constexpr {
        return std::abs(f(x_next));
    };

    return fixed_point_iteration<Dtype>(g, x0, error, settings);
}

#endif //ROOTS_H
