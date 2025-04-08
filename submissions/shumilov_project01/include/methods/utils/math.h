#ifndef UTILS_MATH_H
#define UTILS_MATH_H

#include <cmath>
#include <concepts>

template<std::floating_point scalar_t>
constexpr auto isclose(const scalar_t a, const scalar_t b, const scalar_t rtol = scalar_t{1.0e-05}, const scalar_t atol = scalar_t{1.0e-08}) -> bool
{
   return std::abs(a - b) <= (atol + rtol * std::fabs(b));
}

#endif //UTILS_MATH_H
