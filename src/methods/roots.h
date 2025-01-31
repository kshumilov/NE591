#ifndef ROOTS_H
#define ROOTS_H

#include <concepts>
#include <functional>
#include <cmath>

#include "optimize.h"


template<std::floating_point scalar_t>
constexpr auto newton(
    std::function<scalar_t(scalar_t)> f,
    std::function<scalar_t(scalar_t)> df,
    const scalar_t x0,
    const scalar_t eps = 1.0e-8,
    const std::size_t max_iter = 100U
) -> scalar_t
{
    auto step = [&](const scalar_t x) constexpr -> scalar_t {
        return x - f(x) / df(x);
    };

    std::size_t iter {0};
    auto next = [&](const scalar_t prev, const scalar_t curr) constexpr -> bool {
        return std::abs(prev - curr) > eps && iter++ < max_iter;
    };

    return fixed_point_iteration<scalar_t>(step, x0, next);
}

#endif //ROOTS_H
